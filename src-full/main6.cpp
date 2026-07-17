#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
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
    bool autoNonVnRestore = true;
    bool spellCheckWithDicts = true;
    bool freeMarking = true;
    bool englishWordList = true;
    bool gemini = false;
    bool chromex11 = false;
};

bool is_fcitx5_running() {
    FILE *pipe = popen("pidof fcitx5", "r");
    if (!pipe)
        return false;
    char buf[128];
    bool run = (fgets(buf, sizeof(buf), pipe) != NULL);
    pclose(pipe);
    return run;
}

std::string get_current_im() {
    if (!is_fcitx5_running())
        return "OFF";
    char buf[128];
    std::string res = "";
    FILE *pipe = popen("fcitx5-remote -n 2>/dev/null", "r");
    if (pipe && fgets(buf, 128, pipe))
        res = buf;
    if (pipe)
        pclose(pipe);
    res.erase(res.find_last_not_of(" \n\r\t") + 1);
    return res.empty() ? "OFF" : res;
}

std::string getP() {
    const char *h = std::getenv("HOME");
    return (h) ? std::string(h) + "/.config/fcitx5/conf/vmk.conf"
               : "/tmp/vmk.conf";
}

static bool parseBool(const std::string &v) {
    return v == "True" || v == "true" || v == "1" || v == "Yes" || v == "yes";
}

VmkConfigValues load() {
    VmkConfigValues r;
    std::ifstream f(getP());
    if (!f.is_open())
        return r;

    std::string l;
    while (std::getline(f, l)) {
        size_t p = l.find('=');
        if (p == std::string::npos)
            continue;
        std::string k = l.substr(0, p);
        std::string v = l.substr(p + 1);
        // trim
        while (!k.empty() && (k.back() == ' ' || k.back() == '\t'))
            k.pop_back();

        if (k == "Mode")
            r.mode = v;
        else if (k == "InputMethod")
            r.method = v;
        else if (k == "OutputCharset")
            r.charset = v;
        else if (k == "AutoNonVnRestore")
            r.autoNonVnRestore = parseBool(v);
        else if (k == "SpellCheckWithDicts")
            r.spellCheckWithDicts = parseBool(v);
        else if (k == "FreeMarking")
            r.freeMarking = parseBool(v);
        else if (k == "EnglishWordList")
            r.englishWordList = parseBool(v);
        else if (k == "Gemini")
            r.gemini = parseBool(v);
        else if (k == "ChromeX11")
            r.chromex11 = parseBool(v);
    }
    return r;
}

void save(VmkConfigValues c) {
    std::ofstream f(getP(), std::ios::trunc);
    if (!f.is_open())
        return;

    auto yn = [](bool b) { return b ? "True" : "False"; };

    f << "# Chế độ gõ VMK\n";
    f << "Mode=" << c.mode << "\n";
    f << "InputMethod=" << c.method << "\n";
    f << "OutputCharset=" << c.charset << "\n";
    f << "\n# --- Gõ lẫn tiếng Anh / tiếng Việt ---\n";
    f << "# clear/with/class không bị Telex dính dấu\n";
    f << "AutoNonVnRestore=" << yn(c.autoNonVnRestore) << "\n";
    f << "# Đối chiếu từ điển bamboo\n";
    f << "SpellCheckWithDicts=" << yn(c.spellCheckWithDicts) << "\n";
    f << "# Free marking kiểu UniKey\n";
    f << "FreeMarking=" << yn(c.freeMarking) << "\n";
    f << "# Whitelist EN (builtin + ~/.config/fcitx5/vmk-english-words.txt)\n";
    f << "EnglishWordList=" << yn(c.englishWordList) << "\n";
    f << "\n# --- Fix app ---\n";
    f << "Gemini=" << yn(c.gemini) << "\n";
    f << "ChromeX11=" << yn(c.chromex11) << "\n";
    f.close();

    if (is_fcitx5_running())
        std::system("dbus-send --session --dest=org.fcitx.Fcitx5 /controller "
                    "org.fcitx.Fcitx.Controller1.ReloadConfig &");
}

struct App {
    Fl_Window *w;
    Fl_Button *b, *ok, *run, *kill, *zoom;
    Fl_Choice *c2, *c3, *c4;
    Fl_Check_Button *chkEn, *chkDict, *chkFree, *chkList, *chkGemini, *chkX11;
    Fl_Box *hint;

    void update_status() {
        std::string im = get_current_im();
        if (im == "vmk") {
            b->label("V");
            b->color(fl_rgb_color(255, 80, 80));
        } else if (im == "OFF") {
            b->label("OFF");
            b->color(fl_rgb_color(120, 120, 120));
        } else {
            b->label("E");
            b->color(fl_rgb_color(60, 120, 240));
        }
        b->redraw();
    }

    void apply_scale() {
        int fs = (S < 1.2f) ? 14 : 18;
        FL_NORMAL_SIZE = fs;
        w->size(sc(420), sc(560));
        b->resize((w->w() - sc(100)) / 2, sc(12), sc(100), sc(52));
        b->labelsize((S < 1.2f) ? 20 : 25);
        zoom->resize(w->w() - sc(35) - 10, 10, sc(35), sc(25));

        int cx = sc(150), cw = sc(250), ch = sc(32);
        c2->resize(cx, sc(75), cw, ch);
        c3->resize(cx, sc(115), cw, ch);
        c4->resize(cx, sc(155), cw, ch);

        int y = sc(200);
        chkEn->resize(sc(20), y, sc(380), sc(24));
        y += sc(28);
        chkDict->resize(sc(20), y, sc(380), sc(24));
        y += sc(28);
        chkFree->resize(sc(20), y, sc(380), sc(24));
        y += sc(28);
        chkList->resize(sc(20), y, sc(380), sc(24));
        y += sc(32);
        chkGemini->resize(sc(20), y, sc(380), sc(24));
        y += sc(28);
        chkX11->resize(sc(20), y, sc(380), sc(24));
        y += sc(36);
        hint->resize(sc(20), y, sc(380), sc(40));

        int bw = (w->w() - sc(30)) / 2;
        ok->resize(sc(10), sc(480), bw, sc(40));
        run->resize(sc(20) + bw, sc(480), bw, sc(40));
        kill->resize(sc(10), sc(525), w->w() - sc(20), sc(28));
        w->redraw();
    }
};

int main(int argc, char **argv) {
    if (!is_fcitx5_running()) {
        if (fork() == 0) {
            setsid();
            execlp("fcitx5", "fcitx5", "-d", NULL);
            _exit(0);
        }
    }
    Fl::lock();
    App a;
    a.w = new Fl_Window(420, 560, "VMK Config — gõ EN/VN");
    a.w->color(fl_rgb_color(240, 242, 245));

    a.b = new Fl_Button(0, 0, 0, 0, "...");
    a.b->box(FL_FLAT_BOX);
    a.b->labelcolor(FL_WHITE);
    a.b->labelfont(FL_HELVETICA_BOLD);
    a.b->callback([](Fl_Widget *b, void *) {
        if (!is_fcitx5_running())
            return;
        std::system(std::string(b->label()) == "V"
                        ? "fcitx5-remote -s keyboard-us"
                        : "fcitx5-remote -s vmk");
    });

    a.zoom = new Fl_Button(0, 0, 0, 0, "+");
    a.zoom->callback(
        [](Fl_Widget *, void *v) {
            S = (S < 1.2f) ? 1.4f : 1.0f;
            ((App *)v)->apply_scale();
        },
        &a);

    auto add_c = [](const char *l) {
        Fl_Choice *c = new Fl_Choice(0, 0, 0, 0, l);
        c->box(FL_FLAT_BOX);
        return c;
    };

    a.c2 = add_c("Mode:");
    for (auto x : {"vmk1", "vmk2", "vmkpre", "vmk1hc"})
        a.c2->add(x);

    a.c3 = add_c("Kiểu gõ:");
    // Telex: w chỉ biến u/o/a. Telex W: w đứng một mình → ư
    for (auto x : {"Telex", "Telex W", "VNI"})
        a.c3->add(x);

    a.c4 = add_c("Bảng mã:");
    for (auto x : {"Unicode", "UTF-8", "VNI Windows", "TCVN3 (ABC)",
                   "Unicode tổ hợp"})
        a.c4->add(x);

    a.chkEn = new Fl_Check_Button(0, 0, 0, 0,
                                  " Giữ từ tiếng Anh (clear/with không dính dấu)");
    a.chkDict =
        new Fl_Check_Button(0, 0, 0, 0, " Kiểm tra từ điển tiếng Việt");
    a.chkFree =
        new Fl_Check_Button(0, 0, 0, 0, " Gõ dấu tự do (free marking)");
    a.chkList = new Fl_Check_Button(
        0, 0, 0, 0, " Whitelist từ EN (vmk-english-words.txt)");
    a.chkGemini = new Fl_Check_Button(0, 0, 0, 0, " Gemini / Chrome RichText fix");
    a.chkX11 = new Fl_Check_Button(0, 0, 0, 0, " Chrome X11 fix");

    a.hint = new Fl_Box(0, 0, 0, 0,
                        "Gợi ý: dùng Telex (không Telex W) nếu không muốn W→ư.\n"
                        "Thêm từ EN: ~/.config/fcitx5/vmk-english-words.txt");
    a.hint->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
    a.hint->labelsize(12);
    a.hint->labelcolor(fl_rgb_color(80, 80, 80));

    a.ok = new Fl_Button(0, 0, 0, 0, "Lưu");
    a.ok->box(FL_FLAT_BOX);
    a.ok->color(fl_rgb_color(0, 120, 215));
    a.ok->labelcolor(FL_WHITE);
    a.ok->callback(
        [](Fl_Widget *, void *v) {
            App *ap = (App *)v;
            VmkConfigValues c;
            c.mode = ap->c2->text() ? ap->c2->text() : "vmk1";
            c.method = ap->c3->text() ? ap->c3->text() : "Telex";
            c.charset = ap->c4->text() ? ap->c4->text() : "Unicode";
            c.autoNonVnRestore = (bool)ap->chkEn->value();
            c.spellCheckWithDicts = (bool)ap->chkDict->value();
            c.freeMarking = (bool)ap->chkFree->value();
            c.englishWordList = (bool)ap->chkList->value();
            c.gemini = (bool)ap->chkGemini->value();
            c.chromex11 = (bool)ap->chkX11->value();
            save(c);
        },
        &a);

    a.run = new Fl_Button(0, 0, 0, 0, "Chạy VMK");
    a.run->box(FL_FLAT_BOX);
    a.run->color(fl_rgb_color(68, 71, 70));
    a.run->labelcolor(FL_WHITE);
    a.run->callback([](Fl_Widget *, void *) {
        if (!is_fcitx5_running()) {
            if (fork() == 0) {
                setsid();
                execlp("fcitx5", "fcitx5", "-d", NULL);
                _exit(0);
            }
        }
    });

    a.kill = new Fl_Button(0, 0, 0, 0, "Tắt Fcitx5 / VMK");
    a.kill->box(FL_FLAT_BOX);
    a.kill->color(fl_rgb_color(210, 50, 50));
    a.kill->labelcolor(FL_WHITE);
    a.kill->callback(
        [](Fl_Widget *, void *w) {
            std::system("pkill fcitx5");
            ((Fl_Window *)w)->hide();
        },
        a.w);

    VmkConfigValues v = load();
    auto set_v = [](Fl_Choice *c, const std::string &s) {
        for (int i = 0; i < c->size(); i++)
            if (c->text(i) && s == c->text(i)) {
                c->value(i);
                return;
            }
        c->value(0);
    };
    set_v(a.c2, v.mode);
    set_v(a.c3, v.method);
    set_v(a.c4, v.charset);
    a.chkEn->value(v.autoNonVnRestore);
    a.chkDict->value(v.spellCheckWithDicts);
    a.chkFree->value(v.freeMarking);
    a.chkList->value(v.englishWordList);
    a.chkGemini->value(v.gemini);
    a.chkX11->value(v.chromex11);

    a.apply_scale();
    a.w->end();
    a.w->show(argc, argv);

    std::thread([&a]() {
        while (true) {
            Fl::awake((Fl_Awake_Handler)[](void *d) {
                ((App *)d)->update_status();
            },
                      &a);
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
        }
    }).detach();

    return Fl::run();
}
