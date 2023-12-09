#include "Memory.h"
#include <fstream>
#include <sstream>
#include <sys/uio.h>

pid_t _processId;
unsigned long _moduleBase;

pid_t Memory::GetProcessId(const char* processName) {
    // A buffer for the result
    char buffer[512];

    // Create a string to store the full command to run
    std::string pidof_str = "pidof -s ";
    pidof_str.append(processName);

    // Create a new stream connected to the pipe running the pidof command
    FILE* pidof_pipe = popen(pidof_str.c_str(), "r");

    // Put the result of the pidof command in to the char buffer
    fgets(buffer, 512, pidof_pipe);

    // Read the process id from the char buffer
    pid_t processId = strtoul(buffer, NULL, 10);

    // Cache the found process id
    _processId = processId;

    // Close the pipe
    pclose(pidof_pipe);

    // Return the found process id
    return processId;
}

unsigned long Memory::GetModuleBase(pid_t processId, const char* moduleName) {
    // Return null if the process id is invalid
    if(processId <= 0)
        return 0;

    // Open an incoming file stream to the maps file
    std::ifstream maps_file("/proc/" + std::to_string(processId) + "/maps");

    // If the maps_file couldn't be opened, return null
    if(!maps_file) {
        return 0;
    }

    // Buffer for the line
    std::string line;

    // Loop over all the lines in the maps file
    while(std::getline(maps_file, line)) {
        // Get the position of the first space character
        size_t space_pos = line.rfind(' ');

        // Check if the first space is found
        if(space_pos == std::string::npos)
            continue;

        // Split the string and get the image name
        std::string image_name = line.substr(space_pos + 1);

        std::string module_str = "/";
        module_str.append(moduleName);

        // Check if the image name contains the moduleName
        if(image_name.find(module_str.c_str()) != std::string::npos) {
            // Get the position of the first - character
            size_t dash_pos = line.find('-');

            // Return null if the dash position wasn't found
            if(dash_pos == std::string::npos)
                return 0;
            
            // Get the base address from the line
            std::string base_address_str = line.substr(0, dash_pos);

            // Convert the base address string to an unsigned long
            unsigned long base_address;
            std::stringstream ss(base_address_str);
            ss >> std::hex >> base_address;

            // Cache the module base
            _moduleBase = base_address;

            // Return the found base address
            return base_address;
        }
    }

    // Return null if the base address wasn't found
    return 0;
}

// Get the cached process id
pid_t Memory::GetLastProcessId() {
    return _processId;
}

// Get the cached module base
unsigned long Memory::GetLastModuleBase() {
    return _moduleBase;
}

bool Memory::ReadRaw(unsigned long address, void* buffer, size_t size) {
        // Return false if the size is negative or zero
    if(size <= 0)
        return false;

    // Cast the given address to a void pointer
    void* addressPtr = (void*)address;

    // Get the process id of the last found process
    pid_t processId = GetLastProcessId();

    // Create iovec structs for the process_vm_readv function
    struct iovec iovLocalAddressSpace[1]{0};
    struct iovec iovRemoteAddressSpace[1]{0};

    // Set the values for the local address space
    iovLocalAddressSpace[0].iov_base = buffer;
    iovLocalAddressSpace[0].iov_len = size;

    // Set the values for the remote address space
    iovRemoteAddressSpace[0].iov_base = addressPtr;
    iovRemoteAddressSpace[0].iov_len = size;

    // Read the value at the address and write it to the buffer
    ssize_t returnSize = process_vm_readv(
        processId,
        iovLocalAddressSpace,
        1,
        iovRemoteAddressSpace,
        1,
        0
    );

    // Return true if the return size is the same as the requested size
    if(returnSize == (ssize_t)size) {
        return true;
    }

    // Else return false
    return false;
}

bool Memory::WriteRaw(unsigned long address, void* buffer, size_t size) {
        // Return false if the size is negative or zero
    if(size <= 0)
        return false;

    // Cast the given address to a void pointer
    void* addressPtr = (void*)address;

    // Get the process id of the last found process
    pid_t processId = GetLastProcessId();

    // Create iovec structs for the process_vm_readv function
    struct iovec iovLocalAddressSpace[1]{0};
    struct iovec iovRemoteAddressSpace[1]{0};

    // Set the values for the local address space
    iovLocalAddressSpace[0].iov_base = buffer;
    iovLocalAddressSpace[0].iov_len = size;

    // Set the values for the remote address space
    iovRemoteAddressSpace[0].iov_base = addressPtr;
    iovRemoteAddressSpace[0].iov_len = size;

    // Write the value from the buffer to the given address
    ssize_t returnSize = process_vm_writev(
        processId,
        iovLocalAddressSpace,
        1,
        iovRemoteAddressSpace,
        1,
        0
    );

    // Return true if the return size is the same as the requested size
    if(returnSize == (ssize_t)size) {
        return true;
    }

    // Else return false
    return false;
}

std::string Memory::ReadString(unsigned long address) {
    // Get the size of the std::string type
    int stringSize = sizeof(std::string);

    // Create a buffer to store the chars
    char buffer[stringSize] = {0};

    // Read the chars from the address and write them to the buffer
    bool success = ReadRaw(address, &buffer, stringSize);

    // If it failed, return an empty string
    if(!success)
        return "";
    
    // Else return the std::string of the buffer
    return std::string(buffer);
}