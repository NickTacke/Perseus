#pragma once

namespace KeyManager {
    void AddListener(int keyCode, void (*callback)());
    void AddMultiListener(int keyCode, int secondKey, void (*callback)());
    
    bool RemoveListener(int keyCode);

    void Update();
}