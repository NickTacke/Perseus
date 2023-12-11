#ifndef MEMORY_H
#define MEMORY_H

#include <unistd.h>
#include <string>

namespace Memory {
    // Process id
    pid_t GetProcessId(const char* processName);
    pid_t GetLastProcessId();

    // Module base address
    unsigned long GetModuleBase(pid_t processId, const char* moduleName);
    unsigned long GetLastModuleBase();

    // Read functions
    bool ReadRaw(unsigned long address, void* buffer, size_t size);
    std::string ReadString(unsigned long address);

    // Read template functions
    template <typename T>
    T Read(unsigned long address) {
        T buffer;
        ReadRaw(address, (void*)&buffer, sizeof(T));
        return buffer;
    }
    template <typename T>
    bool ReadArray(unsigned long address, T* array, size_t size) {
        return ReadRaw(address, array, sizeof(T) * size);
    }

    // Write functions
    bool WriteRaw(unsigned long address, void* buffer, size_t size);
    
    // Write template functions
    template <typename T>
    bool Write(unsigned long address, T value) {
        return WriteRaw(address, (void*)&value, sizeof(T));
    }
    template <typename T>
    bool WriteArray(unsigned long address, T* array, size_t size) {
        return WriteRaw(address, array, sizeof(T) * size);
    }
}

#endif // MEMORY_H