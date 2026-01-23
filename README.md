# VMK
Phần mềm gõ tiếng việt VMK
(VMK (Vietnamese Micro Key) for Fcitx5)

VMK là bộ gõ tiếng Việt dành cho Fcitx5, được phát triển với mục tiêu mang lại trải nghiệm gõ non-preedit hoàn hảo trên Linux, mô phỏng chính xác cơ chế hoạt động huyền thoại của UniKey trên Windows.
💡 Tại sao lại là VMK?

Trong nhiều năm qua, người dùng Linux luôn gặp khó khăn với việc gõ tiếng Việt trên Wayland hoặc các ứng dụng Electron (Chrome, Discord...). Các bộ gõ hiện tại thường gây bối rối với quá nhiều chế độ (Modes) hoặc gặp lỗi gạch chân (preedit) gây khó chịu.

VMK giải quyết vấn đề này bằng cách:

    Tái hiện cơ chế UniKey: Sử dụng nguyên lý: Gửi Backspace N lần → Chờ (Delay logic) → Commit chuỗi ký tự.

    Tối ưu cho Wayland: Hoạt động ổn định trên các môi trường desktop hiện đại như GNOME, KDE.

    Đơn giản hóa: Tập trung vào hiệu quả thực tế thay vì cấu hình phức tạp.

    Đặc biệt bộ cài Đơn giản người dùng chỉ cần cài 1 file setup duy nhất và dùng không cần chỉnh config nhiều có thể gõ chữ TV ngay.
### 🛠 Các cơ chế gõ của VMK

VMK không bắt người dùng phải đau đầu lựa chọn hàng chục chế độ. Chúng tôi tập trung tối ưu hóa 3 phương thức cốt lõi để đảm bảo trải nghiệm gõ luôn thông suốt:

## Các Chế Độ Gõ Của VMK

| Chế độ | Tên kỹ thuật | Mô tả cơ chế | Độ tương thích |
| :--- | :--- | :--- | :---: |
| **VMK1** | **UniKey(windows) like** | Mô phỏng cơ chế gửi phím Backspace vật lý để xóa và tái tạo ký tự (giống UniKey trên Windows). Loại bỏ hoàn toàn vùng đệm (preedit). | **> 90%** |
| **VMK1HC** | **VMK1 Enhanced (Hard Core)** | **Phiên bản VMK1 tương thích tốt hơn nữa.** Lưu trạng thái vào `/tmp` (RAM trên SystemD) để giữ engine không bị xóa khi XIM lỗi. | **Tối ưu nhất cho IDE** |
| **VMK2** | **Modern Surrounding** | Sử dụng API `Surrounding Text` kết hợp thuật toán tối ưu của tác giả giúp tốc độ gõ cực nhanh và mượt mà. | **~ 50%** |
| **VMK-Pre** | **Standard Preedit** | Sử dụng preedit truyền thống của Fcitx5. Đây là cơ chế tiêu chuẩn, đóng vai trò phương án dự phòng hoàn hảo. | **100%** |

---

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

Dự án không cung cấp `git clone` trực tiếp. Bạn có thể truy cập mã nguồn qua các thư mục sau:

* **[src-main](./src-main):** Chứa mã nguồn gốc (bao gồm mã nguồn tạo ra `vmk.so`) do tôi phát triển.
https://github.com/user-attachments/files/24425559/0.9apha.zip
* **[src-dependencies](./src-dependencies):** Chứa mã nguồn của các dự án liên quan để tiện cho việc biên dịch chung.
https://github.com/user-attachments/files/24426578/0.9apha_full_for.convenience_.compile.zip
---
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
1. **Di chuyển vào thư mục:** `cd đường_dẫn_đến_thư_mục`
2. **Cấp quyền thực thi:** `chmod +x ./tên_file_script.sh`
3. **Chạy với quyền quản trị:** `sudo ./tên_file_script.sh`

---

ví dụ dành cho deian
[![Download](https://img.shields.io/badge/Download-Debian%20%2F%20Ubuntu-orange?style=for-the-badge&logo=debian)](https://github.com/user-attachments/files/24423654/install_vmk_debian.sh)
* **Lệnh cài nhanh:** 
chmod +x ./install_vmk_debian.sh && sudo ./install_vmk_debian.sh
Nếu bạn đang sử dụng môi trường Desktop KDE trên giao thức Wayland, bạn cần thực hiện thêm bước sau để bộ gõ hoạt động:

    Mở System Settings (hoặc nhấn Search).

    Tìm từ khóa "Virtual Keyboard".

    Tại đây, hãy chọn mục Fcitx 5.
    <img width="1626" height="1099" alt="ảnh" src="https://github.com/user-attachments/assets/f6a32ee1-9216-42b7-bd4f-99151c4470a2" />
<img width="980" height="690" alt="ảnh" src="https://github.com/user-attachments/assets/55328eb4-8f96-44a8-82ba-08600b8eaba6" />
ào home chạy file sconfig để cấu gõ thôi
<img width="866" height="557" alt="ảnh" src="https://github.com/user-attachments/assets/29c84587-8b95-4b2d-b314-9daf90d0f59c" />


### 🐧 1. Debian / Ubuntu (non-GNOME) (Bao gồm cả Linux Mint)
[![Download Debian](https://img.shields.io/badge/Download-Debian%20%2F%20Ubuntu-orange?style=for-the-badge&logo=debian)](https://github.com/user-attachments/files/24423654/install_vmk_debian.sh)
* **Link tải:** [install_vmk_debian.sh](https://github.com/user-attachments/files/24423654/install_vmk_debian.sh)
* **Lệnh cài nhanh:** `wget https://github.com/user-attachments/files/24423654/install_vmk_debian.sh && bash install_vmk_debian.sh`

---

### 🏢 2. Ubuntu GNOME
[![Download Ubuntu](https://img.shields.io/badge/Download-Ubuntu%20GNOME-E94333?style=for-the-badge&logo=ubuntu)](https://github.com/user-attachments/files/24423663/install_vmk_ubuntugnome.sh)
* **Link tải:** [install_vmk_ubuntugnome.sh](https://github.com/user-attachments/files/24423663/install_vmk_ubuntugnome.sh)
* **Lệnh cài nhanh:** `wget https://github.com/user-attachments/files/24423663/install_vmk_ubuntugnome.sh && bash install_vmk_ubuntugnome.sh`

---

### 🔵 3. Fedora
[![Download Fedora](https://img.shields.io/badge/Download-Fedora-51A2DA?style=for-the-badge&logo=fedora)](https://github.com/user-attachments/files/24423665/install_vmk_fedora.sh)
* **Link tải:** [install_vmk_fedora.sh](https://github.com/user-attachments/files/24423665/install_vmk_fedora.sh)
* **Lệnh cài nhanh:** `wget https://github.com/user-attachments/files/24423665/install_vmk_fedora.sh && bash install_vmk_fedora.sh`

---

### 🏹 4. Arch Linux
[![Download Arch](https://img.shields.io/badge/Download-Arch%20Linux-1793D1?style=for-the-badge&logo=arch-linux)](https://github.com/user-attachments/files/24423664/install_vmk_arch.sh)
* **Link tải:** [install_vmk_arch.sh](https://github.com/user-attachments/files/24423664/install_vmk_arch.sh)
* **Lệnh cài nhanh:** `wget https://github.com/user-attachments/files/24423664/install_vmk_arch.sh && bash install_vmk_arch.sh`

---

### 🦎 5. openSUSE
[![Download openSUSE](https://img.shields.io/badge/Download-openSUSE-73BA48?style=for-the-badge&logo=opensuse)](https://github.com/user-attachments/files/24423662/install_vmk_opensuse.sh)
* **Link tải:** [install_vmk_opensuse.sh](https://github.com/user-attachments/files/24423662/install_vmk_opensuse.sh)
* **Lệnh cài nhanh:** `wget https://github.com/user-attachments/files/24423662/install_vmk_opensuse.sh && bash install_vmk_opensuse.sh`
