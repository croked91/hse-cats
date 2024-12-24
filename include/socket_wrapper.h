#ifndef SOCKET_WRAPPER_H
#define SOCKET_WRAPPER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <stdexcept>
#include <cstring>
#include <netdb.h>
#include <unistd.h>

class SocketWrapper {
    int sock;

public:
    SocketWrapper(struct addrinfo* info);
    ~SocketWrapper();

    void connect(struct addrinfo* info);
    int get() const;
};

#endif