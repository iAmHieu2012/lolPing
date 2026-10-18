#!/bin/sh
set -eu
cd "$(dirname "$0")"

CFLAGS=$(pkg-config --cflags Qt6Widgets Qt6Gui Qt6Core Qt6Multimedia Qt6DBus x11 xi)
LIBS=$(pkg-config --libs Qt6Widgets Qt6Gui Qt6Core Qt6Multimedia Qt6DBus x11 xi)

g++ -std=c++20 -O2 -Wall -Wextra -fPIC \
  main.cpp \
  $CFLAGS \
  $LIBS -lLayerShellQtInterface \
  -o lolping-kde

echo "Built $(pwd)/lolping-kde successfully!"
