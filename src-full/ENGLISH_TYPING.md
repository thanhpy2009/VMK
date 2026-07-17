# Chức năng gõ lẫn tiếng Anh / tiếng Việt

Không phải “hack ẩn” — đây là **option chính thức**, bật/tắt được 3 chỗ:

1. **Menu tray Fcitx5** (khi đang dùng VMK)  
2. **`~/sconfig`** (app cấu hình)  
3. **`fcitx5-configtool`** → VMK → các checkbox  
4. File: `~/.config/fcitx5/conf/vmk.conf`

## Các option

| Key trong conf | Tên trên UI | Mặc định | Việc làm |
|----------------|-------------|----------|----------|
| `AutoNonVnRestore` | Giữ từ tiếng Anh | Bật | Từ không phải TV → giữ keystroke (`clear`, …) |
| `SpellCheckWithDicts` | Kiểm tra từ điển TV | Bật | Đối chiếu dict bamboo |
| `FreeMarking` | Gõ dấu tự do | Bật | Free marking kiểu UniKey |
| `EnglishWordList` | Whitelist từ EN | Bật | Ưu tiên không dấu các từ trong list |
| `InputMethod` | Kiểu gõ | **Telex** | `Telex W` mới ép `w` đứng một mình → `ư` |

## Whitelist từ EN

- Builtin ~700 từ (code/chat phổ biến)
- Thêm tay: `~/.config/fcitx5/vmk-english-words.txt` (1 từ / dòng)
- Chỉ có hiệu lực khi **EnglishWordList = True**

## Gợi ý dùng

- Code / chat EN-VN lẫn → bật cả 3: Giữ từ EN + Từ điển + Whitelist, kiểu **Telex**
- Muốn `w` → `ư` như UniKey cũ → chọn **Telex W**
