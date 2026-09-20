# Hoàn tác từ vô nghĩa

Giống UniKey Windows: từ **không hợp lệ** tiếng Việt thì trả lại đúng phím đã gõ; từ **có nghĩa** vẫn bỏ dấu bình thường.

| Gõ | Ý nghĩa | Khi bật hoàn tác |
|----|---------|------------------|
| `no` | vô nghĩa | dừng / trả keystroke |
| `nos` | không phổ biến | hoàn tác |
| `nói` / `hoa` | có nghĩa | bỏ dấu bình thường |
| `clear` → `cleả` | vô nghĩa | hoàn tác → `clear` |

## Option (config / sconfig / menu tray)

| Key | Tên UI | Mặc định |
|-----|--------|----------|
| `AutoNonVnRestore` | Hoàn tác từ vô nghĩa | Bật |
| `SpellCheckWithDicts` | Dùng từ điển khi hoàn tác | Bật |
| `FreeMarking` | Gõ dấu tự do | Bật |
| `EnglishWordList` | Whitelist EN thêm (tuỳ chọn) | **Tắt** |

- Giữa chừng chỉ nhìn **cấu trúc âm tiết** (`IsValid`), không ép list EN (tránh vỡ `no` trong lúc gõ `nói`).
- Khi chốt từ: tra từ điển bamboo nếu bật spell-check.

File conf: `~/.config/fcitx5/conf/vmk.conf`
