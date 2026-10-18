#!/bin/sh
# Installs lolPing KDE Wayland into ~/.local/ and enables KDE Autostart (no root required).
set -eu
cd "$(dirname "$0")"

./build.sh

BIN_DIR="$HOME/.local/bin"
SHARE_DIR="$HOME/.local/share/lolping-kde"
APP_DIR="$HOME/.local/share/applications"
ICON_DIR="$HOME/.local/share/icons/hicolor/128x128/apps"
AUTOSTART_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/autostart"

mkdir -p "$BIN_DIR" "$SHARE_DIR" "$APP_DIR" "$ICON_DIR" "$AUTOSTART_DIR"

install -m 755 lolping-kde "$BIN_DIR/lolping-kde"
rm -rf "$SHARE_DIR/assets"
cp -r ../assets "$SHARE_DIR/assets"
cp ../assets/textures/enemy_missing.png "$ICON_DIR/lolping-kde.png"

cat > "$APP_DIR/lolping-kde.desktop" <<EOF
[Desktop Entry]
Type=Application
Name=lolPing KDE Wayland
Comment=League of Legends Ping & Emote Wheel for KDE Plasma Wayland
Exec="$BIN_DIR/lolping-kde"
Icon=lolping-kde
Terminal=false
Categories=Utility;Game;
EOF

cp "$APP_DIR/lolping-kde.desktop" "$AUTOSTART_DIR/lolping-kde.desktop"
echo "X-KDE-autostart-after=panel" >> "$AUTOSTART_DIR/lolping-kde.desktop"

echo "Installed to $BIN_DIR/lolping-kde"
echo "Assets copied to $SHARE_DIR/assets"
echo "Application menu & KDE Autostart enabled!"
