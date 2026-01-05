#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include <signal.h>

/* --- Cấu hình UI --- */
float S = 1.0f; 
int sc(int v) { return (int)(v * S); }

struct VmkConfigValues { std::string s1, s2, s3, s4; };

// Kiểm tra Fcitx5 chạy
bool is_fcitx5_running() {
    FILE *pid_pipe = popen("pidof fcitx5", "r");
    if (!pid_pipe) return false;
    char buffer[128];
    bool running = (fgets(buffer, sizeof(buffer), pid_pipe) != NULL);
    pclose(pid_pipe);
    return running;
}

/* --- Logic File & IM --- */
std::string getP() {
    const char* h = std::getenv("HOME");
    return (h) ? std::string(h) + "/.config/fcitx5/conf/vmk.conf" : "/tmp/vmk.conf";
}

std::string get_current_im() {
    if (!is_fcitx5_running()) return "OFF";
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen("fcitx5-remote -n 2>/dev/null", "r");
    if (!pipe) return "OFF";
    if (fgets(buffer, 128, pipe) != NULL) result = buffer;
    pclose(pipe);
    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    result.erase(0, result.find_first_not_of(" \n\r\t"));
    return result.empty() ? "OFF" : result;
}

VmkConfigValues load() {
    VmkConfigValues r = {"VN", "vmk1", "Telex", "Unicode"};
    std::ifstream f(getP());
    if (!f.is_open()) return r;
    std::string l; int i = 0;
    while (std::getline(f, l)) {
        i++;
        size_t p = l.find('=');
        if (i == 4) r.s2 = (p != std::string::npos) ? l.substr(p + 1) : "";
        else if (i == 6) r.s3 = (p != std::string::npos) ? l.substr(p + 1) : "";
        else if (i == 8) { r.s4 = (p != std::string::npos) ? l.substr(p + 1) : ""; break; }
    }
    return r;
}

void save(VmkConfigValues c) {
    std::ofstream f(getP(), std::ios::trunc);
    if (!f.is_open()) return;
    f << "# E-V\nTắt-mở=VN\n# Mode\nMode=" << c.s2 
      << "\n# IM\nInputMethod=" << c.s3 << "\n# CS\nOutputCharset=" << c.s4 << "\n";
    f.close();
    if (is_fcitx5_running())
        std::system("dbus-send --session --dest=org.fcitx.Fcitx5 /controller org.fcitx.Fcitx.Controller1.ReloadConfig &");
}

/* --- Giao diện --- */
struct App {
    Fl_Window* w;
    Fl_Button *b, *ok, *run, *kill, *zoom_btn;
    Fl_Choice *c2, *c3, *c4;

    void update_ui_from_system() {
        std::string im = get_current_im();
        if (im == "vmk") {
            b->label("V"); b->color(fl_rgb_color(255, 80, 80)); 
        } else if (im == "OFF") {
            b->label("OFF"); b->color(fl_rgb_color(120, 120, 120)); 
        } else {
            b->label("E"); b->color(fl_rgb_color(60, 120, 240)); 
        }
        b->redraw();
    }

    void apply_scale() {
        int fs = (S < 1.2f) ? 15 : 20;
        FL_NORMAL_SIZE = fs;
        w->size(sc(360), sc(360));
        b->resize((w->w() - sc(100)) / 2, sc(15), sc(100), sc(60));
        b->labelsize((S < 1.2f) ? 20 : 25);
        zoom_btn->resize(w->w() - sc(35) - 10, 10, sc(35), sc(25));
        int cx = sc(110), cw = sc(230), ch = sc(35);
        c2->resize(cx, sc(90), cw, ch); c2->textsize(fs); c2->labelsize(fs);
        c3->resize(cx, sc(135), cw, ch); c3->textsize(fs); c3->labelsize(fs);
        c4->resize(cx, sc(180), cw, ch); c4->textsize(fs); c4->labelsize(fs);
        int bw = (w->w() - sc(30)) / 2;
        ok->resize(sc(10), sc(235), bw, sc(45)); ok->labelsize(fs);
        run->resize(sc(20) + bw, sc(235), bw, sc(45)); run->labelsize(fs);
        kill->resize(sc(10), sc(295), w->w() - sc(20), sc(40)); kill->labelsize(fs);
        w->redraw();
    }
};

void check_status_loop(void* data) {
    App* app = (App*)data;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (true) {
        Fl::awake((Fl_Awake_Handler)[](void* d) { ((App*)d)->update_ui_from_system(); }, app);
        std::this_thread::sleep_for(std::chrono::milliseconds(600)); 
    }
}

int main(int argc, char **argv) {
    // Chỉ tự động mở fcitx5 lần đầu khi khởi động App
    if (!is_fcitx5_running()) {
        if (fork() == 0) {
            setsid();
            execlp("fcitx5", "fcitx5", "-d", NULL);
            _exit(0);
        }
    }

    Fl::set_font(FL_HELVETICA, "-misc-fixed-medium-r-normal--*-*-*-*-*-*-iso10646-1");
    Fl::lock(); 
    App a;
    a.w = new Fl_Window(360, 360, "VMK Config");
    a.w->color(fl_rgb_color(240, 242, 245));

    a.b = new Fl_Button(0,0,0,0, "...");
    a.b->box(FL_FLAT_BOX); a.b->labelcolor(FL_WHITE); a.b->labelfont(FL_HELVETICA_BOLD);
    a.b->callback([](Fl_Widget* btn, void* v){
        if (!is_fcitx5_running()) return;
        if (std::string(btn->label()) == "V") std::system("fcitx5-remote -s keyboard-us &");
        else std::system("fcitx5-remote -s vmk &");
    }, &a);

    a.zoom_btn = new Fl_Button(0,0,0,0, "+");
    a.zoom_btn->callback([](Fl_Widget*, void* v) { S = (S < 1.2f) ? 1.4f : 1.0f; ((App*)v)->apply_scale(); }, &a);

    auto add_c = [](const char* l) { Fl_Choice* c = new Fl_Choice(0,0,0,0,l); c->box(FL_FLAT_BOX); return c; };
    a.c2 = add_c("Mode:"); for(auto x : {"vmk1", "vmk2", "vmkpre"}) a.c2->add(x);
    a.c3 = add_c("Input:"); for(auto x : {"Telex", "VNI", "VIQR", "Telex 2", "other"}) a.c3->add(x);
    a.c4 = add_c("Charset:"); for(auto x : {"Unicode", "UTF-8", "VNI Windows", "TCVN3 (ABC)", "Unicode tổ hợp"}) a.c4->add(x);

    a.ok = new Fl_Button(0,0,0,0,"Save");
    a.ok->box(FL_FLAT_BOX); a.ok->color(fl_rgb_color(0, 120, 215)); a.ok->labelcolor(FL_WHITE);
    a.ok->callback([](Fl_Widget*, void* v) {
        App* ap = (App*)v; VmkConfigValues c;
        c.s2 = ap->c2->text(); c.s3 = ap->c3->text(); c.s4 = ap->c4->text(); save(c);
    }, &a);

    a.run = new Fl_Button(0,0,0,0,"Run VMK");
    a.run->box(FL_FLAT_BOX); a.run->color(fl_rgb_color(68, 71, 70)); a.run->labelcolor(FL_WHITE);
    a.run->callback([](Fl_Widget*, void*) { 
        if (!is_fcitx5_running()) {
            if(fork()==0) { setsid(); execlp("fcitx5", "fcitx5", "-d", NULL); _exit(0); }
        }
    });

    // LOGIC CẬP NHẬT: Vừa kill fcitx5 vừa tắt GUI
    a.kill = new Fl_Button(0,0,0,0,"Turn off VMK");
    a.kill->box(FL_FLAT_BOX); a.kill->color(fl_rgb_color(210, 50, 50)); a.kill->labelcolor(FL_WHITE);
    a.kill->callback([](Fl_Widget*, void* w) { 
        std::system("pkill fcitx5"); 
        ((Fl_Window*)w)->hide(); // Tắt GUI
    }, a.w);

    a.apply_scale();
    VmkConfigValues v_vals = load();
    auto set_v = [](Fl_Choice* c, const std::string& s) {
        for(int i=0; i<c->size(); i++) if(c->text(i) && s == c->text(i)) { c->value(i); return; }
        c->value(0);
    };
    set_v(a.c2, v_vals.s2); set_v(a.c3, v_vals.s3); set_v(a.c4, v_vals.s4);

    a.update_ui_from_system();
    a.w->end(); a.w->show(argc, argv);
    std::thread worker(check_status_loop, &a); 
    worker.detach();
    return Fl::run();
}
