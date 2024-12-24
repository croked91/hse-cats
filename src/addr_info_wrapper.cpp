#include <addr_info_wrapper.h>


AddrInfoWrapper::AddrInfoWrapper(const std::string& host, const std::string& port) : result(nullptr) {
    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), port.c_str(), &hints, &result) != 0) {
        throw std::runtime_error("Failed to resolve host: " + host);
    }
}

AddrInfoWrapper::~AddrInfoWrapper() {
    if (result) {
        freeaddrinfo(result);
    }
}

struct addrinfo* AddrInfoWrapper::get() const {
    return result;
}
