/*
 * SPDX-FileCopyrightText: 2025 Võ Ngô Hoàng Thành <thanhpy2009@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <pwd.h>
#include <thread>
#include <atomic>
#include <poll.h>
#include <sched.h>
#include <sys/resource.h>
#include <libinput.h>
#include <libudev.h>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

// ID sự kiện chuẩn
#define EV_DEVICE_ADDED 1
#define EV_POINTER_BUTTON 402

static int uinput_fd_ = -1;
static std::string MOUSE_FLAG_PATH;
std::atomic<bool> stop_mouse_thread(false);

// --------------------------- HÀM HỆ THỐNG ---------------------------
static void boost_process_priority() {
    setpriority(PRIO_PROCESS, 0, -10);
}

static void pin_to_pcore() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i <= 3; ++i) CPU_SET(i, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
}

static inline void sleep_us(long us) {
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = (us % 1000000) * 1000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, nullptr);
}

// --------------------------- HÀM UINPUT INJECTOR ---------------------------
void send_uinput_event(int type, int code, int value) {
    if (uinput_fd_ < 0) return;
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(uinput_fd_, &ev, sizeof(ev));
}

void send_backspace_uinput(int count) {
    if (uinput_fd_ < 0) return;
    if (count <= 0) return;
    if (count > 15) count = 15; // Đồng bộ với MAX_BACKSPACE_COUNT bên Client

    const int INTER_KEY_DELAY_US = 1200; 

    for (int i = 0; i < count; ++i) {
        send_uinput_event(EV_KEY, KEY_BACKSPACE, 1);
        send_uinput_event(EV_SYN, SYN_REPORT, 0);

        send_uinput_event(EV_KEY, KEY_BACKSPACE, 0);
        send_uinput_event(EV_SYN, SYN_REPORT, 0);

        sleep_us(INTER_KEY_DELAY_US);
    }
}

// --------------------------- MOUSE MONITOR ---------------------------
static const struct libinput_interface interface = {
    .open_restricted = [](const char *path, int flags, void *user_data) -> int {
        return open(path, flags);
    },
    .close_restricted = [](int fd, void *user_data) { close(fd); }
};

void mousePressMonitorThread() {
    struct udev *udev = udev_new();
    struct libinput *li = libinput_udev_create_context(&interface, NULL, udev);
    libinput_udev_assign_seat(li, "seat0");

    auto last_write_time = std::chrono::steady_clock::now();

    while (!stop_mouse_thread.load()) {
        libinput_dispatch(li);
        struct libinput_event *event;
        while ((event = libinput_get_event(li))) {
            int type = (int)libinput_event_get_type(event);

            if (type == EV_DEVICE_ADDED) {
                struct libinput_device *dev = libinput_event_get_device(event);
                if (libinput_device_config_tap_get_finger_count(dev) > 0) {
                    libinput_device_config_tap_set_enabled(dev, LIBINPUT_CONFIG_TAP_ENABLED);
                }
            } 
            else if (type == EV_POINTER_BUTTON) {
                struct libinput_event_pointer *p = libinput_event_get_pointer_event(event);
                if (libinput_event_pointer_get_button_state(p) == 1) { 
                    auto now = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_write_time).count();

                    if (elapsed >= 1000) {
                        std::ofstream flag(MOUSE_FLAG_PATH, std::ios::trunc);
                        if (flag.is_open()) {
                            flag << "Y=1\n";
                            flag.close();
                            last_write_time = now;
                        }
                    }
                }
            }
            libinput_event_destroy(event);
        }
        usleep(5000); 
    }
    libinput_unref(li);
    udev_unref(udev);
}

// --------------------------- MAIN ---------------------------
int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "-u") return 1;
    boost_process_priority();
    pin_to_pcore();

    std::string target_user = argv[2];
    struct passwd* pw = getpwnam(target_user.c_str());
    if (!pw) return 1;

    fs::path base_dir = std::string(pw->pw_dir) + "/.vmksocket";
    fs::create_directories(base_dir);
    (void)chown(base_dir.c_str(), pw->pw_uid, pw->pw_gid);

    MOUSE_FLAG_PATH = (base_dir / ".mouse_flag").string();

    // 1. Setup Uinput
    uinput_fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd_ >= 0) {
        ioctl(uinput_fd_, UI_SET_EVBIT, EV_KEY);
        ioctl(uinput_fd_, UI_SET_KEYBIT, KEY_BACKSPACE);
        struct uinput_user_dev uidev{};
        snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "VMK_Uinput_Server");
        (void)write(uinput_fd_, &uidev, sizeof(uidev));
        ioctl(uinput_fd_, UI_DEV_CREATE);
    }

    // 2. Setup ABSTRACT SOCKET (TÀNG HÌNH ĐỘNG)
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    // Lấy NgàyThángNăm hiện tại
    time_t now = time(0);
    struct tm *ltm = localtime(&now);
    char time_suffix[32];
    strftime(time_suffix, sizeof(time_suffix), "%d%m%Y", ltm);
    std::string socket_name = "vmk_auth_" + std::string(time_suffix) + "_thanhpy";

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    
    // Abstract: byte đầu tiên là '\0'
    addr.sun_path[0] = '\0';
    strncpy(addr.sun_path + 1, socket_name.c_str(), sizeof(addr.sun_path) - 2);

    // Không dùng unlink() vì không có file thật trên disk
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return 1;
    listen(server_fd, 10);

    std::thread(mousePressMonitorThread).detach();
    printf("--- SERVER VMK TÀNG HÌNH ĐANG CHẠY: %s ---\n", socket_name.c_str());

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;
        char buf[256];
        int n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n > 0) {
            buf[n] = 0;
            std::string cmd(buf);
            if (cmd.find("BACKSPACE_") == 0) {
                try {
                    send_backspace_uinput(std::stoi(cmd.substr(10)));
                } catch (...) {}
            }
        }
        close(client_fd);
    }

    if (uinput_fd_ >= 0) {
        ioctl(uinput_fd_, UI_DEV_DESTROY);
        close(uinput_fd_);
    }
    return 0;
}
