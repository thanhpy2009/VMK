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
namespace fs = std::filesystem;

// ID sự kiện chuẩn từ file mẫu của Thành
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

// FIXED BACKSPACE: giữ phím lâu, bắn MSC_SCAN
void send_backspace_uinput(int count) {
    if (uinput_fd_ < 0) return;
    if (count <= 0) return;
    if (count > 10) count = 10;

    const int INTER_KEY_DELAY_US = 1200; // 1ms giữa các backspace

    for (int i = 0; i < count; ++i) {
        send_uinput_event(EV_KEY, KEY_BACKSPACE, 1);
        send_uinput_event(EV_SYN, SYN_REPORT, 0);

        send_uinput_event(EV_KEY, KEY_BACKSPACE, 0);
        send_uinput_event(EV_SYN, SYN_REPORT, 0);

        sleep_us(INTER_KEY_DELAY_US);
    }
}
// --------------------------- MOUSE MONITOR (LOGIC FILE MẪU) ---------------------------
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

    // Biến lưu mốc thời gian cuối cùng ghi file
    auto last_write_time = std::chrono::steady_clock::now();

    printf("--- SERVER: MONITORING (OPTIMIZED DISPATCH) ---\n");

    while (!stop_mouse_thread.load()) {
        // Luôn luôn gọi dispatch để làm sạch hàng đợi, tránh lỗi "system too slow"
        libinput_dispatch(li);
        
        struct libinput_event *event;
        while ((event = libinput_get_event(li))) {
            int type = (int)libinput_event_get_type(event);

            if (type == EV_DEVICE_ADDED) {
                struct libinput_device *dev = libinput_event_get_device(event);
                if (libinput_device_config_tap_get_finger_count(dev) > 0) {
                    libinput_device_config_tap_set_enabled(dev, LIBINPUT_CONFIG_TAP_ENABLED);
                    libinput_device_config_tap_set_button_map(dev, LIBINPUT_CONFIG_TAP_MAP_LRM);
                }
            } 
            else if (type == EV_POINTER_BUTTON) {
                struct libinput_event_pointer *p = libinput_event_get_pointer_event(event);
                if (libinput_event_pointer_get_button_state(p) == 1) { // Pressed
                    
                    auto now = std::chrono::steady_clock::now();
                    // Tính khoảng cách thời gian từ lần ghi trước (ms)
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_write_time).count();

                    // TĂNG DELAY LÊN 1000ms (1 GIÂY) THEO Ý THÀNH
                    if (elapsed >= 1000) {
                        std::ofstream flag(MOUSE_FLAG_PATH, std::ios::trunc);
                        if (flag.is_open()) {
                            flag << "Y=1\n";
                            flag.close();
                            last_write_time = now; // Cập nhật mốc thời gian mới
                            printf(">>> ĐÃ GHI FLAG Y=1 (Đã nghỉ đủ %ld ms)\n", elapsed);
                        }
                    } else {
                        // Nếu chưa đủ 1s thì bỏ qua không ghi, nhưng event vẫn được giải phóng
                        // printf("... Bỏ qua (mới qua %ld ms) ...\n", elapsed);
                    }
                }
            }
            libinput_event_destroy(event);
        }
        
        // Nghỉ 5ms mỗi vòng lặp để giảm tải CPU xuống mức thấp nhất (~0.1%)
        // 5ms vẫn cực nhanh để không bao giờ bị tràn buffer
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
    std::string socket_path = (base_dir / "kb_socket").string();

    // Setup Uinput
    uinput_fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd_ >= 0) {
        ioctl(uinput_fd_, UI_SET_EVBIT, EV_KEY);
        ioctl(uinput_fd_, UI_SET_KEYBIT, KEY_BACKSPACE);
        struct uinput_user_dev uidev{};
        snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Fcitx5_Uinput_Server");
        (void)write(uinput_fd_, &uidev, sizeof(uidev));
        ioctl(uinput_fd_, UI_DEV_CREATE);
    }

    // Setup Socket
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr{.sun_family = AF_UNIX};
    strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);
    unlink(socket_path.c_str());
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
   // xóa dòng quyền 666 cho cập nhật bảo mật
    listen(server_fd, 5);

    // Chạy luồng monitor chuột/touchpad
    std::thread(mousePressMonitorThread).detach();
    printf("--- SERVER ĐANG CHẠY (QUÉT THIẾT BỊ OK) ---\n");

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
    return 0;
}
