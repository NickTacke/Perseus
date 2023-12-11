#ifndef WEBSERVER_H
#define WEBSERVER_H

class WebServer {
public:
    WebServer(int port);
    void start();
    void stop();

private:
    int server_fd;
    int port;
    bool running;
    const int BUFFER_SIZE = 1024;

    void handleRequest(int new_socket);
};

#endif // WEBSERVER_H