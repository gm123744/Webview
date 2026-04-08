#!/bin/bash

set -e
echo "webview installer"

# Root check
if [ "$EUID" -ne 0 ]; then 
    echo "Run this as root: sudo ./install.sh"
    exit 1
fi

read -p "Do you accept the license? (yes/no): " answer

case "$answer" in
    yes|Yes|YES|y|Y)
        echo "License accepted."
        ;;
    *)
        echo "License not accepted. Aborting."
        echo "You can read it in license.txt if you change your mind."
        exit 1
        ;;
esac

if [ ! -f webview.h ]; then
    echo "Error: webview.h not found."
    exit 1
fi

echo "Installing header..."
cp webview.h /usr/local/include/
echo "-> webview.h installed"

if [ -f webview.c ]; then
    echo "Building static library..."

    TMP_DIR=$(mktemp -d)

    gcc -c webview.c -o "$TMP_DIR/webview.o"
    ar rcs "$TMP_DIR/libwebview.a" "$TMP_DIR/webview.o"

    cp "$TMP_DIR/libwebview.a" /usr/local/lib/
    rm -rf "$TMP_DIR"

    echo "-> libwebview.a installed"

    if command -v ldconfig >/dev/null 2>&1; then
        ldconfig
    fi
fi

echo ""
echo "Done. You're good to go."
echo "Example:"
echo "  gcc test.c -lwebview -o test"