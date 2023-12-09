#include "Key.h"
#include "../X11/X11.h"

Key::Key(int keyCode, void (*callback)())
    : keyCode(keyCode), callback(callback) {}

int Key::getKeyCode() {
    return keyCode;
}

void Key::Update() {
    // If the key is down and it shouldn't reset
    if(X11::IsKeyDown(keyCode) && !reset) {
        // Make sure it resets before calling again
        reset = true;

        // Call the given callback
        callback();
    } else if (!X11::IsKeyDown(keyCode) && reset) {
        // Make it so the callback can be executed again
        reset = false;
    }
}