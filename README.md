# VMK
Phần mềm gõ tiếng việt VMK
VMK (Vietnamese Method Key) for Fcitx5

VMK là bộ gõ tiếng Việt dành cho Fcitx5, được phát triển với mục tiêu mang lại trải nghiệm gõ "không vùng đệm" (non-preedit) hoàn hảo trên Linux, mô phỏng chính xác cơ chế hoạt động huyền thoại của UniKey trên Windows.
💡 Tại sao lại là VMK?

Trong nhiều năm qua, người dùng Linux luôn gặp khó khăn với việc gõ tiếng Việt trên Wayland hoặc các ứng dụng Electron (Chrome, Discord...). Các bộ gõ hiện tại thường gây bối rối với quá nhiều chế độ (Modes) hoặc gặp lỗi gạch chân (preedit) gây khó chịu.

VMK giải quyết vấn đề này bằng cách:

    Tái hiện cơ chế UniKey: Sử dụng nguyên lý: Gửi Backspace N lần → Chờ (Delay logic) → Commit chuỗi ký tự.

    Tối ưu cho Wayland: Hoạt động ổn định trên các môi trường desktop hiện đại như GNOME, KDE.

    Đơn giản hóa: Tập trung vào hiệu quả thực tế thay vì cấu hình phức tạp.

🛠 Các cơ chế gõ của VMK

Dự án cung cấp 3 phương thức gõ linh hoạt:
Chế độ	Tên kỹ thuật	Mô tả	Độ tương thích
VMK1	UniKey Classic	Giả lập phím Backspace vật lý để xóa và gửi lại ký tự. Loại bỏ hoàn toàn gạch chân.	> 90%
VMK2	Modern Surrounding	Sử dụng API Surrounding Text của Linux kết hợp thuật toán tối ưu tốc độ của tác giả.	~ 50%
VMK-Pre	Standard Preedit	Cơ chế vùng đệm truyền thống (dựa trên Bamboo engine) dùng để dự phòng.	100%
🏗 Thành phần dự án

Dự án là sự kết hợp tinh hoa từ các mã nguồn mở đi trước:

    Fcitx5 Framework: Phát triển bởi Weng Xuetian (csslayer).

    Bamboo Engine: Phát triển bởi Lâm Quang Lương (Đảm bảo độ chính xác tuyệt đối trong việc bỏ dấu tiếng Việt).

    VMK Core: Do Thành & Thoan phát triển, bổ sung cơ chế Backspace ảo và xử lý Delay logic.
🤝 Đóng góp & Bản quyền

Dự án được phát hành dưới giấy phép GNU GPLv3. Chúng tôi hoan nghênh mọi sự đóng góp từ cộng đồng để cải thiện Delay logic và tăng độ tương thích cho VMK1.

Tác giả:

    Võ Ngô Hoàng Thành (thanhpy2009@gmail.com)

    Nguyễn Văn Thoan
