#include "KeyManager.h"
#include <vector>

#include "Key/Key.h"
#include "Key/MultiKey.h"

// Create a vector to store all the keys
std::vector<Key*> keys;

void KeyManager::AddListener(int keyCode, void (*callback)()) {
    // Add a new instance of Key to the vector
    keys.push_back(new Key(keyCode, callback));
}

bool KeyManager::RemoveListener(int keyCode) {
    bool found = false;

    // Loop over all the Key instances in the vector
    for(int i = 0; i < keys.size(); i++) {
        Key* key = keys.at(i);

        // Don't break/return so it removes every instance
        if(key->getKeyCode() == keyCode) {
            // Remove the key at the current itteration
            keys.erase(keys.begin() + i);
            found = true;
        }
    }

    // Return if a key has been deleted
    return found;
}

void KeyManager::AddMultiListener(int keyCode, int secondKey, void (*callback)()) {
    // Add a new instance of MultiKey to the vector
    keys.push_back(new MultiKey(keyCode, secondKey, callback));
}

void KeyManager::Update() {
    // Loop over all the keys in the vector and call their Update function
    for(Key* key : keys) {
        key->Update();
    }
}