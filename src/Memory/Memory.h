#pragma once
#include <unistd.h>
#include <string>

namespace Memory {
    // Functions for retrieving a process
    pid_t GetProcessId(const char* processName);
    unsigned long GetModuleBase(pid_t processId, const char* moduleName);

    // Cache functions
    pid_t GetLastProcessId();
    unsigned long GetLastModuleBase();

    // Functions for reading and writing raw bytes
    bool ReadRaw(unsigned long address, void* buffer, size_t size);
    bool WriteRaw(unsigned long address, void* buffer, size_t size);

    // Templates for reading and writing memory
    template <typename T>
    T Read(unsigned long address) {
        T buffer;
        ReadRaw(address, (void*)&buffer, sizeof(T));
        return buffer;
    }

    template <typename T>
    void Write(unsigned long address, T value) {
        WriteRaw(address, (void*)&value, sizeof(T));
    }

    // Function to read a string value from memory
    std::string ReadString(unsigned long address);

    // Functions to read and write an array from/to memory
    template <typename T>
    bool ReadArray(unsigned long address, T* array, size_t len) {
        return ReadRaw(address, array, sizeof(T) * len);       
    }

    template <typename T>
    bool WriteArray(unsigned long address, const T* array, size_t len) {
        return WriteRaw(address, array, sizeof(T) * len);
    }
}