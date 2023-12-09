#pragma once

class Key {
protected:
    void (*callback)();
    int keyCode;
    bool reset = false;
public:
    Key(int keyCode, void (*callback)());
    int getKeyCode();
    virtual void Update();
};