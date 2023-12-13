#include "Memory.h"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <sys/uio.h>

pid_t _processId;
unsigned long _moduleBase;

struct memory_transfer_info {
    pid_t procid;          // Process ID
    void *from_buffer;     // Pointer to the source buffer
    void *to_buffer;       // Pointer to the destination buffer
    size_t buffer_size;    // Size of the buffers
};

pid_t Memory::GetProcessId(const char* processName) {
    // Char buffer for command result
    char buffer[512];

    // Full command to get process id
    std::string pidofStr = "pidof -s ";
    pidofStr.append(processName);

    // Run the command
    FILE* pidofPipe = popen(pidofStr.c_str(), "r");

    // Get the response
    fgets(buffer, 512, pidofPipe);

    // Convert the response to an unsigned long
    pid_t processId = strtoul(buffer, NULL, 10);

    // Store the process id in a global variable
    _processId = processId;

    // Close the stream
    pclose(pidofPipe);

    // Return the process id
    return processId;
}

pid_t Memory::GetLastProcessId() {
    // Return the stored process id
    return _processId;
}

// TODO: Use same methods as GetProcessId
unsigned long Memory::GetModuleBase(pid_t processId, const char* moduleName) {
    // Invalid process id check
    if(processId <= 0)
        return NULL;

    // Open the module maps of the given process id
    std::ifstream mapsFile("/proc/" + std::to_string(processId) + "/maps");

    // Check if file opened successfuly
    if(!mapsFile)
        return NULL;

    std::string line;

    while(std::getline(mapsFile, line)) {
        // Get the position of the space character
        size_t spacePos = line.rfind(' ');

        // Check if a space was found
        if(spacePos == std::string::npos)
            continue;

        // Get the name of the current image
        std::string imageName = line.substr(spacePos + 1);

        // Full module name as string
        std::string moduleStr = "/";
        moduleStr.append(moduleName);

        // Check if the current image contains the required module
        if(imageName.find(moduleStr.c_str()) != std::string::npos) {
            // Get the position of the dash
            size_t dashPos = line.find('-');

            // Check if a dash was found
            if(dashPos == std::string::npos)
                return NULL;

            unsigned long baseAddress;
            std::string baseAddressStr = line.substr(0, dashPos);

            // Convert the string to an unsigned long            
            std::stringstream stringStream(baseAddressStr);
            stringStream >> std::hex >> baseAddress;

            // Store the found module base address
            _moduleBase = baseAddress;

            return baseAddress;
        }
    }

    return NULL;
}

unsigned long Memory::GetLastModuleBase() {
    // Return the stored module base address
    return _moduleBase;
}

const char* devicePath = "/dev/Perseus";
int device;

int Memory::OpenCharDriver() {
    // Open the device
    device = open(devicePath, O_RDWR);
    if (device < 0) {
        std::cerr << "Could not open device " << devicePath << std::endl;
        return 1;
    }
}

int Memory::CloseCharDriver() {
    close(device);
    return 0;
}

bool Memory::ReadRaw(unsigned long address, void* buffer, size_t size) {
    // Check for invalid size
    if(size <= 0)
        return false;

    // Required data for reading
    void* addressPtr = (void*)address;
    pid_t processId = GetLastProcessId();

    memory_transfer_info transferInfo = {};
    transferInfo.procid = processId;
    transferInfo.from_buffer = addressPtr;
    transferInfo.to_buffer = buffer;
    transferInfo.buffer_size = size;
    
    ssize_t bytesRead = read(device, (void*)&transferInfo, sizeof(memory_transfer_info));

    // Check if nothing went wrong
    return bytesRead == (ssize_t)size;
}

std::string Memory::ReadString(unsigned long address) {
    // Create a buffer to store individual characters in
    char buffer[sizeof(std::string)] = {0};

    // Read the string bytes
    ReadRaw(address, (void*)&buffer, sizeof(std::string));

    // Return the buffer as a string
    return std::string(buffer);
}

bool Memory::ReadRaw(unsigned long address, void* buffer, size_t size) {
    // Check for invalid size
    if(size <= 0)
        return false;

    // Required data for reading
    void* addressPtr = (void*)address;
    pid_t processId = GetLastProcessId();

    memory_transfer_info transferInfo = {};
    transferInfo.procid = processId;
    transferInfo.from_buffer = buffer;
    transferInfo.to_buffer = addressPtr;
    transferInfo.buffer_size = size;
    
    ssize_t bytesWritten = write(device, &transferInfo, sizeof(memory_transfer_info));

    // Check if nothing went wrong
    return bytesWritten == (ssize_t)size;
}