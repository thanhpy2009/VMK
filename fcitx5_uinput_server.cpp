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
#include <exception>
#include <grp.h>
#include <pwd.h>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <poll.h>
#include <dirent.h>
#include <stdexcept>

// --------------------------- CÁC ĐỊNH NGHĨA VÀ BIẾN TOÀN CỤC ---------------------------
static int uinput_fd_ = -1;
static const int MAX_BACKSPACE_COUNT = 10;

static std::string BASE_DIR; 
static std::string BASE_SOCKET_PATH;
static std::string MOUSE_FLAG_PATH;

static std::string active_socket_path;
std::atomic<bool> stop_mouse_thread(false);

// --------------------------- HÀM HỖ TRỢ SOCKET VÀ QUYỀN ---------------------------

bool is_socket_path_exist(const std::string& path) {
    struct stat buffer;
    if (::stat(path.c_str(), &buffer) == 0) {
        return S_ISSOCK(buffer.st_mode);
    }
    return false;
}

std::string find_available_socket_path() {
    if (BASE_SOCKET_PATH.empty()) return "";
    
    std::string current_path = BASE_SOCKET_PATH;
    if (!is_socket_path_exist(current_path)) return current_path;

    for (int i = 1; i <= 10; ++i) {
        current_path = BASE_SOCKET_PATH + "_" + std::to_string(i);
        if (!is_socket_path_exist(current_path)) return current_path;
    }
    return "";
}

void check_permissions() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) return;
    if (access("/dev/uinput", F_OK) != 0) return;
    if (access("/dev/uinput", R_OK | W_OK) == 0) return;
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
    const int MAX_B_COUNT = 10; 
    if (count > MAX_B_COUNT) count = MAX_B_COUNT;
    
    const int inter_key_delay = 300; 
    for (int i = 0; i < count; ++i) {
        send_uinput_event(EV_KEY, KEY_BACKSPACE, 1); 
        send_uinput_event(EV_SYN, SYN_REPORT, 0); 
        send_uinput_event(EV_KEY, KEY_BACKSPACE, 0);
        send_uinput_event(EV_SYN, SYN_REPORT, 0); 
        usleep(inter_key_delay);
    }
}

int setup_uinput() {
    check_permissions();
    if (uinput_fd_ >= 0) return uinput_fd_;
    
    uinput_fd_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd_ < 0) return -1;
    
    ioctl(uinput_fd_, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd_, UI_SET_EVBIT, EV_SYN);
    ioctl(uinput_fd_, UI_SET_KEYBIT, KEY_BACKSPACE);
    
    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Fcitx5 Input Injector (KB/Mouse)");
    uidev.id.bustype = BUS_VIRTUAL;
    uidev.id.vendor  = 0xABCD;
    uidev.id.product = 0x1234;
    uidev.id.version = 1;

    if (write(uinput_fd_, &uidev, sizeof(uidev)) < 0) {
        close(uinput_fd_);
        uinput_fd_ = -1;
        return -1;
    }
    
    ioctl(uinput_fd_, UI_DEV_CREATE);
    return uinput_fd_;
}

void cleanup_uinput() {
    if (uinput_fd_ >= 0) {
        ioctl(uinput_fd_, UI_DEV_DESTROY);
        close(uinput_fd_);
        uinput_fd_ = -1;
    }
}

// --------------------------- HÀM MOUSE MONITOR (LUỒNG PHỤ) ---------------------------

void mousePressMonitorThread() {
    const std::string user_flag_path = MOUSE_FLAG_PATH;
    std::vector<pollfd> fds;
    std::map<int, std::string> fd_to_name;

    DIR *dir = opendir("/dev/input");
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "event", 5) == 0) {
            std::string path = "/dev/input/" + std::string(entry->d_name);
            int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
            if (fd < 0) continue;
            fds.push_back({fd, POLLIN, 0});
            fd_to_name[fd] = entry->d_name;
        }
    }
    closedir(dir);

    if (fds.empty()) return;

    struct input_event ev;
    while (!stop_mouse_thread.load()) {
        int ret = poll(fds.data(), fds.size(), 100);
        if (ret <= 0) continue;

        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (read(fds[i].fd, &ev, sizeof(ev)) == sizeof(ev)) {
                    if (ev.type == EV_KEY &&
                        (ev.code == BTN_LEFT || ev.code == BTN_RIGHT || ev.code == BTN_MIDDLE) &&
                        ev.value == 1) {
                        
                        int flag_fd = open(user_flag_path.c_str(), O_CREAT | O_WRONLY, 0666);
                        if (flag_fd >= 0) {
                            write(flag_fd, "Y=1\n", 4);
                            close(flag_fd);
                        }
                    }
                }
            }
        }
    }
    
    for (const auto& pfd : fds) close(pfd.fd);
    unlink(user_flag_path.c_str());
}

// --------------------------- HÀM XỬ LÝ LỆNH SOCKET ---------------------------

void handle_client_command(const std::string& command) {
    if (command.rfind("BACKSPACE_", 0) == 0) {
        try {
            int count = std::stoi(command.substr(10));
            send_backspace_uinput(count);
        } catch (...) {}
    }
}

// --------------------------- HÀM MAIN (LUỒNG CHÍNH) ---------------------------

int main(int argc, char* argv[]) {
    if (argc != 3 || std::string(argv[1]) != "-u") return 1;

    std::string target_username = argv[2];
    struct passwd* pw = getpwnam(target_username.c_str());
    if (!pw) return 1;
    
    BASE_DIR = std::string(pw->pw_dir) + "/.vmksocket";
    BASE_SOCKET_PATH = BASE_DIR + "/kb_socket";
    MOUSE_FLAG_PATH = BASE_DIR + "/.mouse_flag";
    active_socket_path = BASE_SOCKET_PATH;
    
    if (mkdir(BASE_DIR.c_str(), 0755) == 0) {
        chown(BASE_DIR.c_str(), pw->pw_uid, pw->pw_gid);
    } else if (errno != EEXIST) {
        return 1;
    }
    
    if (setup_uinput() < 0) return 1;
    
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cleanup_uinput();
        return 1;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, active_socket_path.c_str(), sizeof(addr.sun_path) - 1);

    unlink(active_socket_path.c_str());
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        cleanup_uinput();
        return 1;
    }
    
    chmod(active_socket_path.c_str(), 0666);
    
    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        cleanup_uinput();
        return 1;
    }
    
    std::thread monitor_thread(mousePressMonitorThread);

    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        while (true) {
            char buffer[256];
            int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                buffer[n] = '\0';
                std::string command(buffer);
                if (command == "QUIT") break;
                handle_client_command(command);
            } else {
                break;
            }
        }
        close(client_fd);
    }

    stop_mouse_thread.store(true);
    if (monitor_thread.joinable()) monitor_thread.join();

    cleanup_uinput();
    unlink(active_socket_path.c_str());
    close(server_fd);
    return 0;
}
