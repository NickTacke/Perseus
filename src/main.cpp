#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <math.h>

#include "Web/WebServer.h"

#include "Utils/Math/Vectors.h"
#include "Utils/Memory/Memory.h"

int roundHalfEven(float x) {
    return (x >= 0.0)
        ? static_cast<int>(std::round(x))
        : static_cast<int>(std::round(-x)) * -1;
}

int main() {
    int port = 8080; // Choose your desired port

    WebServer server(port);

    std::thread serverThread([&server]() {
        server.start();
    });

    Memory::OpenCharDriver();

    // Wait for the R5Apex.exe process
    pid_t processId = Memory::GetProcessId("r5apex.exe");
    std::cout << "Found r5apex.exe with process id: " << processId << std::endl;

    // Retrieve the address for the r5apex.exe module
    unsigned long moduleBase = Memory::GetModuleBase(processId, "r5apex.exe");
    std::cout << "Found r5apex.exe module at address: " << std::hex << moduleBase << std::dec << std::endl;

    Vector2 prevPunchAngles = {};
    prevPunchAngles.x = 0;
    prevPunchAngles.y = 0;

    while(true) {
        
        unsigned long localPlayer = Memory::Read<unsigned long>(moduleBase + 0x211fac8);

        Vector2 viewAngles = Memory::Read<Vector2>(localPlayer + 0x2564 - 0x14);

        Vector2 punchAngles = Memory::Read<Vector2>(localPlayer + 0x2468);
        
        Vector2 punchAnglesDiff = {};
        punchAnglesDiff.x = prevPunchAngles.x - punchAngles.x;
        punchAnglesDiff.y = prevPunchAngles.y - punchAngles.y;

        float pitch = (punchAngles.x - prevPunchAngles.x) * 0.8f;
        float yaw = (punchAngles.y - prevPunchAngles.y) * 0.8f;

        if(pitch == 0 && yaw == 0)
            continue;

        prevPunchAngles = punchAngles;

        Vector2 newAngles = {};
        newAngles.x = viewAngles.x - pitch;
        newAngles.y = viewAngles.y - yaw;

        std::cout << pitch << " " << yaw << std::endl;

        if(newAngles.x > 90 || newAngles.x < -90)
            return -1;

        if(newAngles.y > 180 || newAngles.y < -180)
            return -1;

        Memory::Write<Vector2>(localPlayer + 0x25ac - 0x14, newAngles);

        // Wait a millisecond to prevent freezing
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Memory::CloseCharDriver();

    server.stop();

    serverThread.join();

    return -1;
}