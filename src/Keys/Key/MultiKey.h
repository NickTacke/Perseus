#pragma once
#include "Key.h"

class MultiKey : public Key {
    int secondKey;
public:
    MultiKey(int keyCode, int secondKey, void (*callback)());
    void Update() override;
};