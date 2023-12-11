#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

#include "Utils/Memory/Memory.h"

int main() {
    // Wait for the R5Apex.exe process
    pid_t processId = Memory::GetProcessId("r5apex.exe");
    std::cout << "Found r5apex.exe with process id: " << processId << std::endl;

    // Retrieve the address for the r5apex.exe module
    unsigned long moduleBase = Memory::GetModuleBase(processId, "r5apex.exe");
    std::cout << "Found r5apex.exe module at address: " << std::hex << moduleBase << std::dec << std::endl;

    while(true) {
        // moduleBase + localPlayer + m_iHealth
        std::cout << Memory::ReadString(moduleBase + 0x16966f0) << std::endl;

        // Wait a millisecond to prevent freezing
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return -1;
}