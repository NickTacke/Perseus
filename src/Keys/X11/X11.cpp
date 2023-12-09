#include "X11.h"

// Get a pointer to the current display
Display* display = XOpenDisplay(NULL);

bool X11::IsKeyDown(int keyCode) {
    // Create a buffer to store the keys
    char buffer[32];

    // Get the keymap from the display and store it in the buffer
    XQueryKeymap(display, buffer);

    // Translate the KeySym to a KeyCode
    KeyCode translated = XKeysymToKeycode(display, keyCode);

    // Return if the keyCode is true in the buffer
    return (!!(buffer[translated >> 3] & (1 << (translated & 7))));
}