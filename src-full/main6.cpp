#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <thread>

/* --- Cấu hình UI --- */
float S = 1.0f; 
int sc(int v) { return (int)(v * S); }

struct VmkConfigValues { 
    std::string mode, method, charset; 
    bool gemini, chromex11; 
};

// Kiểm tra trạng thái Fcitx5
bool is_fcitx5_running() {
    FILE *pipe = popen("pidof fcitx5", "r");
    if (!pipe) return false;
    char buf[128];
    bool run = (fgets(buf, sizeof(buf), pipe) != NULL);
    pclose(pipe);
    return run;
}

std::string get_current_im() {
    if (!is_fcitx5_running()) return "OFF";
    char buf[128];
    std::string res = "";
    FILE* pipe = popen("fcitx5-remote -n 2>/dev/null", "r");
    if (pipe && fgets(buf, 128, pipe)) res = buf;
    if (pipe) pclose(pipe);
    res.erase(res.find_last_not_of(" \n\r\t") + 1);
    return res.empty() ? "OFF" : res;
}

/* --- LOGIC FILE --- */
std::string getP() {
    const char* h = std::getenv("HOME");
    return (h) ? std::string(h) + "/.config/fcitx5/conf/vmk.conf" : "/tmp/vmk.conf";
}

VmkConfigValues load() {
    VmkConfigValues r = {"vmk1", "Telex", "Unicode", false, false};
    std::ifstream f(getP());
    if (!f.is_open()) return r;
    
    std::string l;
    while (std::getline(f, l)) {
        size_t p = l.find('=');
        if (p == std::string::npos) continue;
        std::string k = l.substr(0, p);
        std::string v = l.substr(p + 1);

        if (k == "Mode") r.mode = v;
        else if (k == "InputMethod") r.method = v;
        else if (k == "OutputCharset") r.charset = v;
        else if (k == "Gemini") r.gemini = (v == "True");
        else if (k == "ChromeX11") r.chromex11 = (v == "True");
    }
    return r;
}

void save(VmkConfigValues c) {
    std::ofstream f(getP(), std::ios::trunc);
    if (!f.is_open()) return;
    f << "# Chế độ\nMode=" << c.mode << "\n";
    f << "# Input Method\nInputMethod=" << c.method << "\n";
    f << "# Output Charset\nOutputCharset=" << c.charset << "\n";
    f << "# Gemini-fix\nGemini=" << (c.gemini ? "True" : "False") << "\n";
    f << "# Chrome X11 Fix\nChromeX11=" << (c.chromex11 ? "True" : "False") << "\n";
    f.close();
    
    if (is_fcitx5_running())
        std::system("dbus-send --session --dest=org.fcitx.Fcitx5 /controller org.fcitx.Fcitx.Controller1.ReloadConfig &");
}

/* --- UI --- */
struct App {
    Fl_Window* w;
    Fl_Button *b, *ok, *run, *kill, *zoom;
    Fl_Choice *c2, *c3, *c4;
    Fl_Check_Button *chk1, *chk2;

    void update_status() {
        std::string im = get_current_im();
        if (im == "vmk") { b->label("V"); b->color(fl_rgb_color(255, 80, 80)); }
        else if (im == "OFF") { b->label("OFF"); b->color(fl_rgb_color(120, 120, 120)); }
        else { b->label("E"); b->color(fl_rgb_color(60, 120, 240)); }
        b->redraw();
    }

    void apply_scale() {
        int fs = (S < 1.2f) ? 15 : 20;
        FL_NORMAL_SIZE = fs;
        w->size(sc(360), sc(400));
        b->resize((w->w() - sc(100)) / 2, sc(15), sc(100), sc(60));
        b->labelsize((S < 1.2f) ? 20 : 25);
        zoom->resize(w->w() - sc(35) - 10, 10, sc(35), sc(25));
        int cx = sc(110), cw = sc(230), ch = sc(35);
        c2->resize(cx, sc(90), cw, ch);
        c3->resize(cx, sc(130), cw, ch);
        c4->resize(cx, sc(170), cw, ch);
        chk1->resize(cx, sc(210), cw, sc(25));
        chk2->resize(cx, sc(235), cw, sc(25));
        int bw = (w->w() - sc(30)) / 2;
        ok->resize(sc(10), sc(275), bw, sc(45));
        run->resize(sc(20) + bw, sc(275), bw, sc(45));
        kill->resize(sc(10), sc(335), w->w() - sc(20), sc(40));
        w->redraw();
    }
};

int main(int argc, char **argv) {
    if (!is_fcitx5_running()) {
        if (fork() == 0) { setsid(); execlp("fcitx5", "fcitx5", "-d", NULL); _exit(0); }
    }
    Fl::lock();
    App a;
    a.w = new Fl_Window(360, 400, "VMK Config");
    a.w->color(fl_rgb_color(240, 242, 245));

    a.b = new Fl_Button(0,0,0,0,"...");
    a.b->box(FL_FLAT_BOX); a.b->labelcolor(FL_WHITE); a.b->labelfont(FL_HELVETICA_BOLD);
    a.b->callback([](Fl_Widget* b, void*){
        if (!is_fcitx5_running()) return;
        std::system(std::string(b->label()) == "V" ? "fcitx5-remote -s keyboard-us" : "fcitx5-remote -s vmk");
    });

    a.zoom = new Fl_Button(0,0,0,0,"+");
    a.zoom->callback([](Fl_Widget*, void* v){ S = (S < 1.2f) ? 1.4f : 1.0f; ((App*)v)->apply_scale(); }, &a);

    auto add_c = [](const char* l) { Fl_Choice* c = new Fl_Choice(0,0,0,0,l); c->box(FL_FLAT_BOX); return c; };
    
    a.c2 = add_c("Mode:"); 
    for(auto x : {"vmk1", "vmk2", "vmkpre", "vmk1hc"}) a.c2->add(x);

    // CẬP NHẬT CHỈ CÒN 3 OPTION THEO ẢNH
    a.c3 = add_c("Input:"); 
    for(auto x : {"Telex", "Telex W", "VNI"}) a.c3->add(x);

    a.c4 = add_c("Charset:"); 
    for(auto x : {"Unicode", "UTF-8", "VNI Windows", "TCVN3 (ABC)", "Unicode tổ hợp"}) a.c4->add(x);

    a.chk1 = new Fl_Check_Button(0,0,0,0, " Gemini Fix");
    a.chk2 = new Fl_Check_Button(0,0,0,0, " Chrome X11");

    a.ok = new Fl_Button(0,0,0,0,"Save");
    a.ok->box(FL_FLAT_BOX); a.ok->color(fl_rgb_color(0, 120, 215)); a.ok->labelcolor(FL_WHITE);
    a.ok->callback([](Fl_Widget*, void* v){
        App* ap = (App*)v;
        save({ap->c2->text(), ap->c3->text(), ap->c4->text(), (bool)ap->chk1->value(), (bool)ap->chk2->value()});
    }, &a);

    a.run = new Fl_Button(0,0,0,0,"Run VMK");
    a.run->box(FL_FLAT_BOX); a.run->color(fl_rgb_color(68, 71, 70)); a.run->labelcolor(FL_WHITE);
    a.run->callback([](Fl_Widget*, void*){ if(!is_fcitx5_running()) { if(fork()==0){setsid(); execlp("fcitx5","fcitx5","-d",NULL); _exit(0);}} });

    a.kill = new Fl_Button(0,0,0,0,"Turn off VMK");
    a.kill->box(FL_FLAT_BOX); a.kill->color(fl_rgb_color(210, 50, 50)); a.kill->labelcolor(FL_WHITE);
    a.kill->callback([](Fl_Widget*, void* w){ std::system("pkill fcitx5"); ((Fl_Window*)w)->hide(); }, a.w);

    VmkConfigValues v = load();
    auto set_v = [](Fl_Choice* c, const std::string& s) {
        for(int i=0; i<c->size(); i++) if(c->text(i) && s == c->text(i)) { c->value(i); return; }
        c->value(0);
    };
    set_v(a.c2, v.mode); set_v(a.c3, v.method); set_v(a.c4, v.charset);
    a.chk1->value(v.gemini); a.chk2->value(v.chromex11);

    a.apply_scale();
    a.w->end(); a.w->show(argc, argv);
    
    std::thread([&a](){
        while(true) {
            Fl::awake((Fl_Awake_Handler)[](void* d){ ((App*)d)->update_status(); }, &a);
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
        }
    }).detach();

    return Fl::run();
}
