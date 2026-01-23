src full for convenience compile
Bamboo (Vietnamese Input Method) engine support for Fcitx(folder fcitx5-vmk.0.9apha/bamboo)
Based on https://github.com/BambooEngine/bamboo-core
Bambootogo
https://github.com/fcitx/fcitx5-bamboo(folder fcitx5-vmk.0.9apha/bamboo)
Released under LGPLv2.1+
Based on https://github.com/thanhpy2009/VMK (fodler fcitx5-vmk.0.9aphasrc)
Released under GPL v3.0
all of other files on this for folder 
Based on https://github.com/thanhpy2009/VMK 
Released under GPL v3.0
Requires:
Fcitx 5, go.

Install:
$ mkdir build && cd build
$ cmake -DCMAKE_INSTALL_PREFIX=[prefix to fcitx5] ..
$ make
# make install
Bamboo (Vietnamese Input Method) engine support for Fcitx
g++ -std=gnu++17 -O2 -pthread fcitx5_uinput_server.cpp -o fcitx5_uinput_server
g++ -Os main6.cpp -o sconfig -lpthread -I./fltk-1.3.9 -static-libgcc -static-libstdc++ ./fltk-1.3.9/lib/libfltk.a -lX11 -lXext -lpthread -ldl -lm -lc && strip --strip-all sconfig

