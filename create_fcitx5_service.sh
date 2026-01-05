#!/bin/bash

# --- CẤU HÌNH ---
TARGET_USER=${SUDO_USER:-$(whoami)}
PROXY_USER="uinput_proxy"
DATA_DIR="/home/$TARGET_USER/.vmksocket"
SERVICE_NAME="fcitx5-uinput-server.service"
SERVICE_FILE="/etc/systemd/system/$SERVICE_NAME"
SERVER_SOURCE="./fcitx5_uinput_server"
SERVER_DESTINATION="/usr/local/bin/fcitx5_uinput_server"

# --- KIEM TRA QUYEN ROOT ---
if [ "$EUID" -ne 0 ]; then
  echo "⚠️ LOI: Script phai duoc chay voi quyen root (su dung sudo)."
  exit 1
fi

# --- 0. DỪNG DỊCH VỤ VÀ GỠ BỎ TỆP CŨ ---
echo "--- 0. DUNG DICH VU VA GO BO TEP CU ---"
systemctl stop "$SERVICE_NAME" 2>/dev/null
systemctl disable "$SERVICE_NAME" 2>/dev/null
rm -f "$SERVICE_FILE"

if [ ! -f "$SERVER_SOURCE" ]; then
    echo "❌ LOI: Khong tim thay file thuc thi Server: $SERVER_SOURCE."
    exit 1
fi

# --- 1. KIỂM TRA/TẠO USER PROXY ---
echo "--- 1. TAO USER PROXY ($PROXY_USER) ---"
id -u "$PROXY_USER" &>/dev/null || useradd -r -M -s /usr/sbin/nologin "$PROXY_USER"
usermod -aG input "$PROXY_USER"
usermod -aG "$TARGET_USER" "$PROXY_USER"

# --- 1.5. NAP KERNEL MODULE UINPUT ---
echo "--- 1.5. NAP KERNEL MODULE UINPUT ---"
# Nạp ngay lập tức cho phiên hiện tại
modprobe uinput

# Cấu hình nạp vĩnh viễn khi khởi động lại
if [ ! -f /etc/modules-load.d/uinput.conf ]; then
    echo "uinput" | tee /etc/modules-load.d/uinput.conf > /dev/null
    echo "✅ Da cau hinh tu dong nap module uinput khi khoi dong."
fi
# --- 2. CẤU HÌNH UDEV ---
echo "--- 2. CAU HINH RULE UDEV CHO /dev/uinput ---"

# Ghi rule chuẩn vào file (Dùng OPTIONS+="static_node=uinput" để ép hệ thống nhận diện)
echo 'KERNEL=="uinput", SUBSYSTEM=="misc", OPTIONS+="static_node=uinput", MODE="0660", GROUP="input"' | sudo tee /etc/udev/rules.d/99-uinput.rules > /dev/null

# Nạp lại cấu hình udev
sudo udevadm control --reload-rules
sudo udevadm trigger
echo "Kiem tra quyen /dev/uinput hien tai:"
ls -l /dev/uinput

# --- 3. CẤP QUYỀN TRUY CẬP (ACL) ---
echo "--- 3. THIET LAP QUYEN TRUY CAP ---"
mkdir -p "$DATA_DIR"
chown "$TARGET_USER":"$TARGET_USER" "$DATA_DIR"

# Quyền 'x' để Proxy đi xuyên qua thư mục Home (An toàn)
setfacl -m u:"$PROXY_USER":x "/home/$TARGET_USER"
# Quyền 'rwx' tuyệt đối bên trong thư mục socket
setfacl -R -m u:"$PROXY_USER":rwx "$DATA_DIR"
setfacl -R -m d:u:"$PROXY_USER":rwx "$DATA_DIR"
setfacl -R -m u:"$TARGET_USER":rwx "$DATA_DIR"

# --- 4. CÀI ĐẶT FILE THỰC THI ---
echo "--- 4. CAI DAT FILE THUC THI SERVER ---"
cp "$SERVER_SOURCE" "$SERVER_DESTINATION"
chown root:root "$SERVER_DESTINATION"
chmod 755 "$SERVER_DESTINATION"

# --- 5. TẠO FILE SERVICE SYSTEMD ---
echo "--- 5. TAO FILE SERVICE SYSTEMD ---"
cat << EOF > "$SERVICE_FILE"
[Unit]
Description=Fcitx5 Limited Uinput Server
After=network.target multi-user.target udev.target

[Service]
User=$PROXY_USER
Group=input
Type=simple
WorkingDirectory=/tmp
Environment=HOME=/home/$TARGET_USER
Environment=DATA_DIR=$DATA_DIR

# ExecStartPre phải nằm trong phần [Service] của file này
ExecStartPre=/usr/bin/sleep 1
ExecStart=$SERVER_DESTINATION -u $TARGET_USER

# Cấp lại quyền cho các file socket sau khi server tạo ra chúng
ExecStartPost=/bin/sh -c "/bin/sleep 0.5 && /usr/bin/setfacl -m u:$TARGET_USER:rw,m:rwx $DATA_DIR/kb_socket $DATA_DIR/.mouse_flag || true"

Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

# --- 6. KÍCH HOẠT SYSTEMD ---
echo "--- 6. TAI LAI VA KHOI DONG SERVICE ---"
systemctl daemon-reload
systemctl enable "$SERVICE_NAME"
systemctl restart "$SERVICE_NAME"

echo "---"
if systemctl is-active --quiet "$SERVICE_NAME"; then
    echo "✅ HOAN THANH! Dịch vụ đang chạy."
else
    echo "❌ LOI: Dịch vụ không khởi động được. Kiểm tra: journalctl -xeu $SERVICE_NAME"
fi
