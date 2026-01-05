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

| Chế độ | Tên kỹ thuật | Mô tả cơ chế | Độ tương thích |
| :--- | :--- | :--- | :---: |
| **VMK1** | **UniKey Classic** | Mô phỏng cơ chế gửi phím Backspace vật lý để xóa và tái tạo ký tự (như UniKey trên Windows). Đây là chế độ chủ đạo giúp loại bỏ hoàn toàn vùng đệm (preedit). | **> 90%** |
| **VMK2** | **Modern Surrounding** | Sử dụng API `Surrounding Text` của hệ thống để xóa/thay thế văn bản. Được kết hợp thuật toán tối ưu của tác giả giúp tốc độ gõ cực nhanh và mượt mà. | **~ 50%** |
| **VMK-Pre** | **Standard Preedit** | Sử dụng preedit truyền thống của Fcitx5. Đây là cơ chế tiêu chuẩn của hệ điều hành, đóng vai trò là phương án dự phòng hoàn hảo cho mọi ứng dụng. | **100%** |

---

### 🏗 Thành phần & Nguồn gốc dự án

VMK là một bước tiến mới dựa trên sự kế thừa và kết hợp tinh hoa từ các dự án mã nguồn mở xuất sắc:

* **Fcitx5 Framework:** Nền tảng bộ gõ mạnh mẽ và hiện đại nhất trên Linux/Wayland, phát triển bởi **Weng Xuetian (csslayer)**.
* **Bamboo Engine:** Trình xử lý tiếng Việt thông minh với thuật toán bỏ dấu chuẩn xác, phát triển bởi **Lâm Quang Lương (luongthanhlam)**.
* **VMK Core:** Phần lõi do **Võ Ngô Hoàng Thành & Nguyễn Văn Thoan** phát triển. Chúng tôi tập trung bổ sung cơ chế **Backspace ảo** và **Logic tính toán độ trễ (Delay)** nhằm mang lại cảm giác gõ "không vùng đệm" quen thuộc của người dùng Windows lên Linux.
🤝 Đóng góp & Bản quyền

Dự án được phát hành dưới giấy phép GNU GPLv3. Chúng tôi hoan nghênh mọi sự đóng góp từ cộng đồng để cải thiện Delay logic và tăng độ tương thích cho VMK1.

Tác giả:

    Võ Ngô Hoàng Thành (thanhpy2009@gmail.com)

    Nguyễn Văn Thoan
   ## 📋 Danh sách các phiên bản hỗ trợ

Script và mã nguồn được thiết kế để hoạt động tốt nhất trên các phiên bản hệ điều hành mới nhất tính đến ngày **05/01/2026**:

* **Ubuntu:** Phiên bản **24.04 (Noble Numbat)** trở về sau.
* **Debian:** Phiên bản **13 (Trixie)** trở về sau.
* **Fedora:** Phiên bản **43** trở về sau.
* **Arch Linux:** Các bản cài đặt từ ngày **05/01/2026** trở về sau.
* **openSUSE Tumbleweed:** Các bản cập nhật từ ngày **05/01/2026** trở về sau.

### ⚙️ Yêu cầu tương thích
Để đảm bảo bộ gõ và các tính năng hoạt động ổn định, hệ thống cần đáp ứng:
* **Fcitx5:** Phiên bản **5.1.7** trở về sau.
* **Quyền quản trị:** Cần quyền `sudo` để thực thi các lệnh cài đặt gói hệ thống. 
## 🛠️ Quy trình cài đặt tự động

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

Chọn phiên bản phù hợp với hệ điều hành của bạn:

### 🐧 1. Debian / Ubuntu (non-GNOME)
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
