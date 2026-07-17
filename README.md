# VMK
Phần mềm gõ tiếng việt VMK
(VMK (Vietnamese Micro Key) for Fcitx5)
# chú ý các bạn khởi động lại sau khi cài đặt, mới dùng được nha
VMK là bộ gõ tiếng Việt dành cho Fcitx5, được phát triển với mục tiêu mang lại trải nghiệm gõ non-preedit hoàn hảo trên Linux, mô phỏng chính xác cơ chế hoạt động huyền thoại của UniKey trên Windows.

Phần mềm hiện đang ở giai đoạn alpha. Tác giả biết Bug từ hoạt động hay bảo mật có thể còn nên mới đặt bản alpha. Nếu có lỗi thực sự nào hãy nhấn nút issue tôi sẽ reply trở lại trong 24h hoặc hơn chút. Hi vọng các bạn không thích đừng spam. Có vẻ Một số bạn kì vọng quá cao về dự án, cũng như người làm dự án.  

Ghi chú: Phiên bản 0.9.31 là phiên bản vá lỗi 666 trong file cpp ngay từ đầu đã không có lỗi bảo mật vì file quản lý serivce mới là bên set quyền
 
 Cập nhật Phiên bản 0.9.5 demo. Dùng Dbus áp dụng check Dbus với mức độ bảo mật cao để không dùng file socket nữa. Dù sao file socket biến mất vẫn tốt hơn bảo mật 2 lớp =)) 
<p align="center">
  <a href="https://youtu.be/-pCALOigHts">
    <img src="https://img.youtube.com/vi/-pCALOigHts/hqdefault.jpg" alt="Xem Video">
  </a>
</p>

Thấy mọi người kì vọng ghê Luôn. Mình chỉ amature làm đến mức này là quá sức. Mình không có nhiều khả năng như mọi người nghĩ. Mình thấy các phần mềm của tập đoàn lớn họ cập nhật bảo mật cũng cần 1 tháng. Đây không phải là lỗ hổng bảo mật lớn như mọi người nghĩ. Cảm ơn mọi người nha patch này mình chỉ ra với dạng file thử nghiệm. Mình sẽ test trước khi phát hành 1 tuần tức tuần sau mới có file cài với dạng thử nghiệm. Mình thấy là ổn hơn bảo mật 2 lớp bản thân Dbus nếu cấu hình tốt là độ bảo mật cao rồi được tin cậy trong linux. 

Việc phát hành Source và đóng gói các bản sau đều do người khác đảm nhiệm . Và có tên trong readme. Sau khi ra 0.9.5 thử nghiệm. Dự kiến sẽ không ra bản mới 2-3 tháng tới. Không phải vì drama mà kế hoạch từ đầu. Mình chỉ code một mạch đạt tới unikey-clone thì mình dừng. 
Cảm ơn các bạn đã ủng hộ
 
💡 Tại sao lại là VMK?

Trong nhiều năm qua, người dùng Linux luôn gặp khó khăn với việc gõ tiếng Việt trên Wayland hoặc các ứng dụng Electron (Chrome, Discord...). Các bộ gõ hiện tại thường gây bối rối với quá nhiều chế độ (Modes) hoặc gặp lỗi gạch chân (preedit) gây khó chịu.

VMK giải quyết vấn đề này bằng cách:

    Tái hiện cơ chế UniKey: Sử dụng nguyên lý: Gửi Backspace N lần → Chờ (Delay logic) → Commit chuỗi ký tự.

    Tối ưu cho Wayland: Hoạt động ổn định trên các môi trường desktop hiện đại như GNOME, KDE.

    Đơn giản hóa: Tập trung vào hiệu quả thực tế thay vì cấu hình phức tạp.

    Đặc biệt bộ cài Đơn giản người dùng chỉ cần cài 1 file setup duy nhất và dùng không cần chỉnh config nhiều có thể gõ chữ TV ngay.
    Lưu ý mình ko Rep bất cứ lỗi trên FB nếu ko có ticket
### 🛠 Các cơ chế gõ của VMK
    Có hướng dẫn chi tiết test chi tiết patch note chi tiết

    https://www.youtube.com/@ThanhThanh-it1pm

VMK không bắt người dùng phải đau đầu lựa chọn hàng chục chế độ. Chúng tôi tập trung tối ưu hóa 3 phương thức cốt lõi để đảm bảo trải nghiệm gõ luôn thông suốt:

## Các Chế Độ Gõ Của VMK

| Chế độ | Tên kỹ thuật | Mô tả cơ chế | Độ tương thích |
| :--- | :--- | :--- | :---: |
| **VMK1** | **UniKey(windows) like** | Mô phỏng cơ chế gửi phím Backspace vật lý để xóa và tái tạo ký tự (giống UniKey trên Windows). Loại bỏ hoàn toàn vùng đệm (preedit). | **> 90%** |
| **VMK1HC** | **VMK1 Enhanced (Hard Core)** | **Phiên bản VMK1 tương thích tốt hơn nữa. Sẽ thay thế VMK1 trong các bản sau** Lưu trạng thái vào `/tmp` (RAM trên SystemD) để giữ engine không bị xóa khi XIM lỗi. | **Tối ưu nhất cho IDE** |
| **VMK2** | **Modern Surrounding** | Sử dụng API `Surrounding Text` kết hợp thuật toán tối ưu của tác giả giúp tốc độ gõ cực nhanh và mượt mà. | **~ 50%** |
| **VMK-Pre** | **Standard Preedit** | Sử dụng preedit truyền thống của Fcitx5. Đây là cơ chế tiêu chuẩn, đóng vai trò phương án dự phòng hoàn hảo. | **100%** |

---
## 🚀 VMK 0.9.31 Alpha – Patch Notes
Src code được dẫn link đến folder gốc nếu bạn tải src về tự tải thư mục https://github.com/fcitx/fcitx5-bamboo/tree/main/bamboo về bỏ file VMK
/src-full để compile nhé. Mình không hỗ trợ gì thêm vì có vẽ git không cho phép link về thư muc sub
Bản cập nhật quan trọng tập trung vào tối ưu hóa kiến trúc phần cứng hiện đại và sửa lỗi tương thích trên các trình duyệt phổ biến.

### 🛠️ Cải tiến hệ thống (Improvements)
* **Thuật toán Delay thông minh:** Tối ưu hóa logic phản hồi phím, mang lại cảm giác gõ mượt mà và giảm thiểu tối đa lỗi nhập liệu (input lag).
* **IDE Specialized Fix:** Tích hợp cơ chế sửa lỗi sai dấu dành riêng cho các môi trường lập trình (JetBrains, VS Code) bằng VMK1HC.

### ⚡ Hiệu suất & Phần cứng (Performance)
* **E-core Optimization:** Khắc phục triệt để lỗi Race Condition do hệ thống đưa tiến trình vào nhân hiệu suất thấp (E-core) trên các dòng CPU mới.
* **CPU Usage:** Sửa lỗi rò rỉ hiệu năng gây chiếm dụng 100% CPU trên một nhân đơn lẻ.

### 🌐 Sửa lỗi Trình duyệt (Browser Fixes)
* **Google Chrome:**
  * Sửa lỗi lặp ký tự (double character) trên thanh địa chỉ do xung đột cơ chế autofill trình duyệt chrome based
  * Khắc phục lỗi mất ký tự đầu tiên khi gõ trang **Gemini** Chrome based trong KDE.
  * Vì lỗi biến mất ký tự trên thanh địa chỉ cho người dùng **Linux Mint (X11)**. Tôi tạm thời có 1 chế độ chrome X11 bạn tích vào chrome được chuyển sang chế độ preedit tự động X11 nếu chọn VMK1/VMk1hc nếu bạn bị mất chữ thì tích vào các phần mềm khác sẽ về chế độ không preedit tự động không cần chọn lại,  
note: vmk ko trực tiếp fix khi gõ thanh địa chỉ firefox tuy nhiên có cách chỉnh setting firefox. Tôi hướng dẫn trong video

---
### 🌐 Sửa lỗi cấp quyền bảo mật 666 file sevrer
### 📥 Cách cập nhật lên 0.9.31 Alpha
link tải bên dưới. Cài mà không cần gỡ bản cũ. Nó tự gỡ và cài bản mới vào
### Hoàn tác từ vô nghĩa

Giống UniKey: từ không hợp lệ (`no`, `cleả`…) trả lại keystroke; từ có nghĩa (`nói`, `hoa`) vẫn bỏ dấu bình thường.

Bật trong config / `sconfig` / menu tray:

- **Hoàn tác từ vô nghĩa** (`AutoNonVnRestore`)
- **Dùng từ điển khi hoàn tác** (`SpellCheckWithDicts`)

Kiểu gõ nên dùng **Telex** (không phải Telex W) nếu không muốn `w` đứng một mình thành `ư`.

Chi tiết: [src-full/HOAN_TAC_TU_VO_NGHIA.md](src-full/HOAN_TAC_TU_VO_NGHIA.md)

### Thông Tin Chi Tiết Về VMK1HC (Hard Core)
Chế độ này được thiết kế đặc biệt cho các môi trường làm việc chuyên sâu:
* **Cơ chế:** Lưu trạng thái gõ vào thư mục `/tmp`. Đối với các distro sử dụng **systemd**, dữ liệu này sẽ nằm trực tiếp trên RAM.
* **Mục tiêu:** Khắc phục tình trạng engine bị xóa khi Fcitx5 mất kết nối với các phần mềm do giao thức XIM quá lỗi thời.
* **Ứng dụng:** Giúp các IDE phổ biến như **PyCharm, IntelliJ IDEA, Android Studio, VS Code...** hoạt động ổn định và mượt mà hơn rất nhiều.

### Thử Nghiệm Trên Môi Trường Wine (Bonus)
Mặc dù VMK không hỗ trợ chính thức cho Wine, nhưng qua thực nghiệm thực tế VMK1HC:
* Hoạt động tốt với các ứng dụng: **MS Office 2013/2007**, **Notepad++**, **Zalo (Proton)**...
* Người dùng có thể sử dụng trực tiếp VMK mà không cần cài đặt thêm bộ gõ UniKey trên Wine.
* **Lưu ý:** Đây là kết quả thử nghiệm cá nhân, không có đảm bảo chính thức vì Wine không phải là môi trường Linux native.

---

---

### 🏗 Thành phần & Nguồn gốc dự án

VMK là một bước tiến mới dựa trên sự kế thừa và kết hợp tinh hoa từ các dự án mã nguồn mở xuất sắc:

* **Fcitx5 Framework:** Nền tảng bộ gõ mạnh mẽ và hiện đại nhất trên Linux/Wayland, phát triển bởi **Weng Xuetian (csslayer)**.
* **Bamboo Engine:** Trình xử lý tiếng Việt thông minh với thuật toán bỏ dấu chuẩn xác, phát triển bởi **Lâm Quang Lương (luongthanhlam)**.
* **VMK Core:** Phần lõi do **Võ Ngô Hoàng Thành & Nguyễn Văn Thoan** phát triển. Chúng tôi tập trung bổ sung cơ chế **Backspace ảo** và **Logic tính toán độ trễ (Delay)** nhằm mang lại cảm giác gõ "không vùng đệm" quen thuộc của người dùng Windows lên Linux.
🤝 Đóng góp & Bản quyền
(Hiện chưa git clone được nhé các bạn thông cảm)
Dự án được phát hành dưới giấy phép GNU GPLv3. Chúng tôi hoan nghênh mọi sự đóng góp từ cộng đồng để cải thiện Delay logic và tăng độ tương thích cho VMK1.

Tác giả:

    Võ Ngô Hoàng Thành (thanhpy2009@gmail.com)

    Nguyễn Văn Thoan
<p align="center">
  <img src="https://github.com/user-attachments/assets/1db94b23-3a75-4266-b122-d41a196aed71" width="300" >
</p>
    Nếu bạn thây dùng tốt Donate cho tác giả ly cafe 
    
   ## 📋 Danh sách các phiên bản hỗ trợ

Script và mã nguồn được thiết kế để hoạt động tốt nhất trên các phiên bản hệ điều hành mới nhất tính đến ngày **05/01/2026**:
Hiện tại mình Nói rõ là bộ gõ hoạt động tốt với Wayland (Gnome và KDE bản mới) còn X11 chỉ hỗ trợ Linux Mint(XFCE và cinnamon)
* **Ubuntu:** Phiên bản **24.04 (Noble Numbat)** trở về sau. Hiện ubuntu tại Gnome.
* **Debian:** Phiên bản **13 (Trixie)** trở về sau.
* **Fedora:** Phiên bản **43** trở về sau.
* **Arch Linux:** Các bản cài đặt từ ngày **05/01/2026** trở về sau.
* **openSUSE LEAF:** 
  Dùng tốt nhất trên KDE và Gnome wayland bản mới nhất. Ngoài linux Mint cũng là lựa chọn tối ưu 

### ⚙️ Yêu cầu tương thích
Để đảm bảo bộ gõ và các tính năng hoạt động ổn định, hệ thống cần đáp ứng:
* **Fcitx5:** Phiên bản **5.1.7** trở về sau.
* **Quyền quản trị:** Cần quyền `sudo` để thực thi các lệnh cài đặt gói hệ thống. 
## 🛠️ Quy trình cài đặt tự động
Trước khi cài VMK bạn cần gõ các gói IBUS nếu đã cài
Khi chạy script, hệ thống sẽ thực hiện các bước sau:

1.  **Cài đặt Fcitx5 từ kho ứng dụng:** Tải các gói `fcitx5`, `fcitx5-gtk`, `fcitx5-qt`, `fcitx5-lua` (Phiên bản ≥ **5.1.7**).
2.  **Cài đặt Plugin hệ thống:** * Copy file thư viện **`vmk.so`** và các file `.conf` cần thiết vào thư mục **Addon** của Fcitx5 (đường dẫn được script tự động nhận diện tương ứng với từng Distro).
3.  **Cấu hình Người dùng:**
    * Copy file **`sconfig`** vào thư mục Home (`~/sconfig`) để người dùng có thể điều khiển và tùy chỉnh bộ gõ.
4.  **Kích hoạt Service hỗ trợ:** Thiết lập dịch vụ chạy ngầm để xử lý cơ chế gửi phím **Backspace** thông minh, giúp xóa dấu chính xác và ổn định như Unikey.

---

## 📂 Cấu trúc Mã nguồn (Source Code)

Dự án không cung cấp `git clone` trực tiếp.


Video hướng dẫn chung cho các distro mình lấy ubuntu/debian làm ví dụ.
Chú ý KDE wayland bạn phải vào ô search->virtual keyboard chọn fcitx5 như trong video hướng dẫn mới gõ đc
<p align="center">
  <a href="https://www.youtube.com/watch?v=EKQmJMjzgQs">
    <img src="https://img.youtube.com/vi/EKQmJMjzgQs/hqdefault.jpg" alt="Xem Video">
  </a>
</p>
## 📥 Chọn phiên bản phù hợp

## 📥 Tải xuống & Cài đặt



Sau khi tải script về, bạn hãy mở Terminal tại thư mục chứa file và chạy các lệnh sau:

### 🛠️ Các bước thực hiện chung:
#### Bước 0: Chuẩn bị hệ thống
Để VMK hoạt động tốt nhất và tránh xung đột bộ gõ, bạn nên thực hiện các chuẩn bị sau:

* **Khuyến nghị về IBus:** Bạn nên cân nhắc gỡ bỏ hoàn toàn IBus khỏi hệ thống để tránh xung đột phím tắt và engine gõ.
* **Tắt Fcitx5 đang chạy:** Trước khi cài đặt phiên bản mới, hãy đóng an toàn phiên bản hiện tại bằng lệnh D-Bus:
  ```bash
  dbus-send --type=method_call --dest=org.fcitx.Fcitx5 /controller org.fcitx.Fcitx.Controller1.Exit
1. **Di chuyển vào thư mục:** `cd đường_dẫn_đến_thư_mục`
2. **Cấp quyền thực thi:** `chmod +x ./tên_file_script.sh`
3. **Chạy với quyền quản trị:** `sudo ./tên_file_script.sh`

---
Khởi động lại vào Terminal 
fcitx5 
ctrl+space để chuyển bộ gõ. chi tiết hơn sẽ có trong video
ví dụ dành cho deian
[![Download](https://img.shields.io/badge/Download-Debian%20%2F%20Ubuntu-orange?style=for-the-badge&logo=debian)](https://github.com/user-attachments/files/24423654/install_vmk_debian.sh)
* **Lệnh cài nhanh:** 
  dbus-send --type=method_call --dest=org.fcitx.Fcitx5 /controller org.fcitx.Fcitx.Controller1.Exit
chmod +x ./install_vmk_debian.sh && sudo ./install_vmk_debian.sh
Nếu bạn đang sử dụng môi trường Desktop KDE trên giao thức Wayland, bạn cần thực hiện thêm bước sau để bộ gõ hoạt động:

    Mở System Settings (hoặc nhấn Search).

    Tìm từ khóa "Virtual Keyboard".

    Tại đây, hãy chọn mục Fcitx 5.
    <img width="1626" height="1099" alt="ảnh" src="https://github.com/user-attachments/assets/f6a32ee1-9216-42b7-bd4f-99151c4470a2" />
<img width="980" height="690" alt="ảnh" src="https://github.com/user-attachments/assets/55328eb4-8f96-44a8-82ba-08600b8eaba6" />
ào home chạy file sconfig để cấu gõ thôi
<img width="866" height="557" alt="ảnh" src="https://github.com/user-attachments/assets/29c84587-8b95-4b2d-b314-9daf90d0f59c" />
VMK_0.9.31 alpha

## VMK Installer Scripts
### 🐧 1. Debian / Ubuntu (non-GNOME) (Bao gồm cả Linux Mint)
[![Download Debian](https://img.shields.io/badge/Download-Debian%20%2F%20Ubuntu-orange?style=for-the-badge&logo=debian)](https://github.com/user-attachments/files/24973274/install_vmk_debian_0.9.31.sh)
* **Link tải:** [install_vmk_debian_0.9.31.sh](https://github.com/user-attachments/files/24973274/install_vmk_debian_0.9.31.sh)


### 🏢 2. Ubuntu 24.04 (GNOME)
[![Ubuntu GNOME](https://img.shields.io/badge/Ubuntu%2024.04-Coming%20Soon-grey?style=for-the-badge&logo=ubuntu)](https://github.com/thanhpy2009/VMK)
* **Trạng thái:** 🕒 **Coming Soon** (Đang cập nhật bản vá cho GNOME)

---

### 🔵 3. Fedora
[![Download Fedora](https://img.shields.io/badge/Download-Fedora-51A2DA?style=for-the-badge&logo=fedora)](https://github.com/user-attachments/files/24973275/install_vmk_fedora_0.9.31.sh)
* **Link tải:** [install_vmk_fedora_0.9.31.sh](https://github.com/user-attachments/files/24973275/install_vmk_fedora_0.9.31.sh)

---

### 🏹 4. Arch Linux
[![Download Arch](https://img.shields.io/badge/Download-Arch%20Linux-1793D1?style=for-the-badge&logo=arch-linux)](https://github.com/user-attachments/files/24973277/install_vmk_arch_0.9.31.sh)
* **Link tải:** [install_vmk_arch_0.9.31.sh](https://github.com/user-attachments/files/24973277/install_vmk_arch_0.9.31.sh)

---

### 🦎 5. openSUSE
[![Download openSUSE](https://img.shields.io/badge/Download-openSUSE-73BA48?style=for-the-badge&logo=opensuse)](https://github.com/user-attachments/files/24973276/install_vmk_opensuse_0.9.31.sh)
* **Link tải:** [install_vmk_opensuse_0.9.31.sh](https://github.com/user-attachments/files/24973276/install_vmk_opensuse_0.9.31.sh)

---

Uninstaller dành cho cả 4 distro
* **Link tải:** https://github.com/user-attachments/files/24910982/Uninstaller_vmk.sh



## 🛠️ Cấu hình biến môi trường (Environment Variables)

Nếu bạn gặp vấn đề không gõ được trong một số phần mềm, hãy thêm các biến môi trường để tương thích tốt hơn.  
*(Lưu ý: GNOME Wayland thường không cần bước này).*

### 1. Cấu hình cho Người dùng (User level)
Mở tệp bằng lệnh: `nano ~/.profile` và thêm các dòng sau vào cuối:

```bash
export GTK_IM_MODULE=fcitx
export QT_IM_MODULE=fcitx
export XMODIFIERS=@im=fcitx
export SDL_IM_MODULE=fcitx
export GLFW_IM_MODULE=ibus
2. Cấu hình trong​

Mở tệp bằng quyền root: sudo nano /etc/environment và thêm:



GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
SDL_IM_MODULE=fcitx
GLFW_IM_MODULE=ibus
