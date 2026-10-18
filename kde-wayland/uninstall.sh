#!/bin/sh
# Uninstalls lolPing KDE Wayland from ~/.local/ and removes Autostart entry.
set -eu

pkill -x lolping-kde 2>/dev/null || true
rm -f "$HOME/.local/bin/lolping-kde"
rm -rf "$HOME/.local/share/lolping-kde"
rm -f "$HOME/.local/share/applications/lolping-kde.desktop"
rm -f "$HOME/.local/share/icons/hicolor/128x128/apps/lolping-kde.png"
rm -f "${XDG_CONFIG_HOME:-$HOME/.config}/autostart/lolping-kde.desktop"

echo "Uninstalled lolPing KDE Wayland."
