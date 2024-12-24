#ifndef ADDR_INFO_WRAPPER_H
#define ADDR_INFO_WRAPPER_H

#include <string>
#include <netdb.h>
#include <stdexcept>

class AddrInfoWrapper {
    struct addrinfo* result;

public:
    AddrInfoWrapper(const std::string& host, const std::string& port);
    ~AddrInfoWrapper();

    struct addrinfo* get() const;
};

#endif