#!/bin/bash

# --- CẤU HÌNH ---
TARGET_USER=${SUDO_USER:-$(whoami)}
PROXY_USER="${TARGET_USER}_proxy"
DATA_DIR="/home/$TARGET_USER/.vmksocket"
SERVICE_NAME="fcitx5-uinput-$TARGET_USER.service"
SERVICE_FILE="/etc/systemd/system/$SERVICE_NAME"
SERVER_SOURCE="./fcitx5_uinput_server"
SERVER_DESTINATION="/usr/local/bin/fcitx5_uinput_server"

# --- KIỂM TRA QUYỀN ROOT ---
if [ "$EUID" -ne 0 ]; then
  echo "⚠️ LỖI: Script phải được chạy với quyền root (sử dụng sudo)."
  exit 1
fi

# --- 0. DỪNG DỊCH VỤ CŨ ---
echo "--- 0. DUNG DICH VU VA GO BO TEP CU ---"
systemctl stop "$SERVICE_NAME" 2>/dev/null
systemctl disable "$SERVICE_NAME" 2>/dev/null

if [ ! -f "$SERVER_SOURCE" ]; then
    echo "❌ LOI: Khong tim thay file thuc thi Server: $SERVER_SOURCE."
    exit 1
fi

# --- 1. KIỂM TRA/TẠO USER PROXY ---
echo "--- 1. TAO USER PROXY ($PROXY_USER) ---"
getent group input >/dev/null || groupadd input
id -u "$PROXY_USER" &>/dev/null || useradd -r -M -s /usr/sbin/nologin "$PROXY_USER"
usermod -aG input "$PROXY_USER"
usermod -aG "$TARGET_USER" "$PROXY_USER"

# --- 2. NAP KERNEL MODULE & UDEV ---
echo "--- 2. CAU HINH UINPUT & UDEV ---"
modprobe uinput
echo "uinput" | tee /etc/modules-load.d/uinput.conf > /dev/null

# Rule hỗ trợ cả uinput và libinput (input group)
cat << EOF > /etc/udev/rules.d/99-uinput.rules
KERNEL=="uinput", SUBSYSTEM=="misc", OPTIONS+="static_node=uinput", MODE="0660", GROUP="input"
SUBSYSTEM=="input", MODE="0660", GROUP="input"
EOF

udevadm control --reload-rules
udevadm trigger

# --- 3. THIẾT LẬP QUYỀN TRUY CẬP (ACL) ---
echo "--- 3. THIET LAP QUYEN TRUY CAP ACL ---"
mkdir -p "$DATA_DIR"
chown "$TARGET_USER:$TARGET_USER" "$DATA_DIR"
chmod 700 "$DATA_DIR"

setfacl -m u:"$PROXY_USER":x "/home/$TARGET_USER"
setfacl -R -m u:"$PROXY_USER":rwx "$DATA_DIR"
setfacl -R -m d:u:"$PROXY_USER":rwx "$DATA_DIR"

# --- 4. CÀI ĐẶT FILE THỰC THI ---
echo "--- 4. CAI DAT FILE THUC THI SERVER ---"
cp "$SERVER_SOURCE" "$SERVER_DESTINATION"
chown root:root "$SERVER_DESTINATION"
chmod 755 "$SERVER_DESTINATION"

# --- 5. TẠO FILE SERVICE SYSTEMD (FORMAT MOI) ---
echo "--- 5. TAO FILE SERVICE SYSTEMD ---"
cat << EOF > "$SERVICE_FILE"
[Unit]
Description=Fcitx5 Limited Uinput Server for $TARGET_USER
After=network.target multi-user.target udev.target

[Service]
User=$PROXY_USER
Group=input
# Capability hỗ trợ libinput/uinput cấp cao
CapabilityBoundingSet=CAP_DAC_OVERRIDE CAP_SYS_ADMIN
AmbientCapabilities=CAP_DAC_OVERRIDE CAP_SYS_ADMIN
Type=simple
WorkingDirectory=/tmp
Environment=HOME=/home/$TARGET_USER
Environment=DATA_DIR=$DATA_DIR

ExecStartPre=/usr/bin/sleep 1
ExecStart=$SERVER_DESTINATION -u $TARGET_USER

# Cấp quyền trực tiếp cho socket file sau khi khởi tạo
ExecStartPost=/usr/bin/sh -c "/usr/bin/sleep 0.5 && /usr/bin/setfacl -m u:$TARGET_USER:rw,u:$PROXY_USER:rw $DATA_DIR/kb_socket $DATA_DIR/.mouse_flag || true"

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
    echo "✅ HOAN THANH! Dịch vụ cho $TARGET_USER đang chạy."
else
    echo "❌ LOI: Kiểm tra journalctl -xeu $SERVICE_NAME"
fi
