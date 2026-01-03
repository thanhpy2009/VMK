# VMK
Phần mềm gõ tiếng việt VMK
VMK (Vietnamese Micro Key) for Fcitx5

VMK là bộ gõ tiếng Việt dành cho Fcitx5, được phát triển với mục tiêu mang lại trải nghiệm gõ non-preedit hoàn hảo trên Linux, mô phỏng chính xác cơ chế hoạt động huyền thoại của UniKey trên Windows.
💡 Tại sao lại là VMK?

Trong nhiều năm qua, người dùng Linux luôn gặp khó khăn với việc gõ tiếng Việt trên Wayland hoặc các ứng dụng Electron (Chrome, Discord...). Các bộ gõ hiện tại thường gây bối rối với quá nhiều chế độ (Modes) hoặc gặp lỗi gạch chân (preedit) gây khó chịu.

VMK giải quyết vấn đề này bằng cách:

    Tái hiện cơ chế UniKey: Sử dụng nguyên lý: Gửi Backspace N lần → Chờ (Delay logic) → Commit chuỗi ký tự.

    Tối ưu cho Wayland: Hoạt động ổn định trên các môi trường desktop hiện đại như GNOME, KDE.

    Đơn giản hóa: Tập trung vào hiệu quả thực tế thay vì cấu hình phức tạp.
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
