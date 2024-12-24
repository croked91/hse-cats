#include <socket_wrapper.h>

SocketWrapper::SocketWrapper(struct addrinfo* info) : sock(socket(info->ai_family, info->ai_socktype, info->ai_protocol)) {
    if (sock < 0) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    struct timeval timeout;
    timeout.tv_sec = 16;  // Тайм-аут 16 секунд. Т.к. по условию задержка от 1 до 15 секунд. Плюс 1 сек на запас.
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error("Failed to set receive timeout: " + std::string(strerror(errno)));
    }
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        throw std::runtime_error("Failed to set send timeout: " + std::string(strerror(errno)));
    }
}

SocketWrapper::~SocketWrapper() {
    if (sock >= 0) {
        close(sock);
    }
}

void SocketWrapper::connect(struct addrinfo* info) {
    if (::connect(sock, info->ai_addr, info->ai_addrlen) < 0) {
        throw std::runtime_error("Failed to connect to socket");
    }
}

int SocketWrapper::get() const {
    return sock;
}