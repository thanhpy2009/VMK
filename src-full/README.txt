
I can't keep src full for convenience compile anymore. I Must to link to OG project but I can't link sub-fordler on git
src-full require:download 
1.'src-full/bamboo' from
Based on https://github.com/fcitx/fcitx5-bamboo/tree/main/bamboo
'src-full/data/vietnamese.cm.dict' form ﻿https://github.com/fcitx/fcitx5-bamboo/blob/main/data/vietnamese.cm.dict
Bambootogo
wengxt Released under LGPLv2.1+
2.'src-full/bamboo/bamboo-core'
Based on https://github.com/BambooEngine/bamboo-core
The MIT License (MIT) Copyright (C) 2018 Luong Thanh Lam
 https://github.com/BambooEngine/bamboo-core
3. folder Src + fcitx5_uinput_server+main6.cpp 
is my project src code. Vo Ngo Hoang Thanh thanhpy2009 
Based on https://github.com/thanhpy2009/VMK 
Released under GPL v3.0
Requires:
Fcitx 5, go,libinput, fltk-1.3.9
 
Install:
$ mkdir build && cd build
$ cmake -DCMAKE_INSTALL_PREFIX=[prefix to fcitx5] ..
$ make
# make install
Bamboo (Vietnamese Input Method) engine support for Fcitx
g++ -std=gnu++17 -O2 -pthread fcitx5_uinput_server.cpp -o fcitx5_uinput_server
g++ -Os main6.cpp -o sconfig -lpthread -I./fltk-1.3.9 -static-libgcc -static-libstdc++ ./fltk-1.3.9/lib/libfltk.a -lX11 -lXext -lpthread -ldl -lm -lc && strip --strip-all sconfig

Note on Code Integration: "Since this project involves manually merging components from three different sources to achieve the unique 'no-preedit' mechanism, some legacy code snippets or original comments may still remain in the files. These are unintentional leftovers from the integration process. I am working to clean up the source code, but my primary focus remains on the functional stability of the input method. I appreciate your understanding as I refine the project structure."
I am a hardware engineer and a regular Linux user. My knowledge of Git and official open-source repository management is limited. I use Git primarily through terminal commands to download and install tools.
Disclaimer from the Author
This project was born out of a desire to fix the "preedit" issue for the Vietnamese community. If there are any concerns regarding how the code is structured or linked, please understand it is due to my technical limitations with Git, not an intent to claim others' work. I welcome any guidance from the community to help me align this project with standard practices.
