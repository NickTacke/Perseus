#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace X11 {
    bool IsKeyDown(int keyCode);
}