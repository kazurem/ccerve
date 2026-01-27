#pragma once

#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


// Creating sockets
inline static int createSocket(int domain, int type, int protocol) noexcept(true) {
    return socket(domain, type, protocol);
}

// Closing sockets
inline static int closeSocket(int sock) {
    return close(sock);
}