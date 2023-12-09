#include "MultiKey.h"
#include "../X11/X11.h"

MultiKey::MultiKey(int keyCode, int secondKey, void (*callback)()) 
    : Key(keyCode, callback), secondKey(secondKey) {}

void MultiKey::Update() {
    // If the key is down and it shouldn't reset
    if((X11::IsKeyDown(keyCode) && X11::IsKeyDown(secondKey))&& !reset) {
        // Make sure it resets before calling again
        reset = true;

        // Call the given callback
        callback();
    } else if (!(X11::IsKeyDown(keyCode) && X11::IsKeyDown(secondKey)) && reset) {
        // Make it so the callback can be executed again
        reset = false;
    }
}