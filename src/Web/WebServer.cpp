#include "WebServer.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

WebServer::WebServer(int port) : port(port), running(false) {
    // Initialize other variables if needed
}

void WebServer::start() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char* response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1>Hello, World!</h1>";

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Setsockopt failed" << std::endl;
        return;
    }

    // Prepare the sockaddr_in structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return;
    }

    running = true;

    while (running) {
        int new_socket;
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            return;
        }

        // Handle the request
        handleRequest(new_socket);

        close(new_socket);
    }

    stop();
}

void WebServer::stop() {
    running = false;
    close(server_fd);
}

void WebServer::handleRequest(int new_socket) {
    const char* response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<h1>Hello, World!</h1>";
    char buffer[BUFFER_SIZE] = {0};

    // Read the request from the client
    read(new_socket, buffer, BUFFER_SIZE);
    std::cout << "Received request:\n" << buffer << std::endl;

    // Send the response to the client
    send(new_socket, response, strlen(response), 0);
    std::cout << "Response sent" << std::endl;
}