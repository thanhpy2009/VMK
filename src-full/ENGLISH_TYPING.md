# Gõ lẫn tiếng Anh khi dùng VMK

Một số bạn (nhất là dev) hay gặp:

- gõ `clear` bị dính dấu vì phím `r` / free-marking
- gõ `w` đứng một mình thành `ư` nếu đang để **Telex W**

## Mặc định đề xuất (bản patch này)

| Tuỳ chọn | Mặc định | Ý nghĩa |
|----------|----------|---------|
| AutoNonVnRestore | bật | Từ không hợp lệ TV → trả lại keystroke gốc |
| SpellCheckWithDicts | bật | Đối chiếu từ điển bamboo |
| FreeMarking | bật | Vẫn gõ dấu thoải mái như UniKey |
| Input method | **Telex** | `w` chỉ biến `u/o/a` → `ư/ơ/ă`, **không** ép `w` → `ư` |

Nếu thích kiểu UniKey `w` → `ư` mọi lúc, chọn lại **Telex W** trong menu kiểu gõ.

## Ghi chú

Phần engine bamboo (wordlist tiếng Anh mở rộng) nằm ở `fcitx5-bamboo` / `bamboo-core`.
Patch này chỉ chỉnh **mặc định + config phía VMK** — đủ để phần lớn case coding/chat đỡ “bể” chữ Anh.
