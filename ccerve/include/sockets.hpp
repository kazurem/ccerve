#pragma once

/**
 * @file sockets.hpp
 * @brief Hold implementations for socket specific utilities. For now,
 * the functions are small enough that they are implemented directly
 * in this file. This may change in the future if cross-platform
 * support is implemented with all the conditional compilation stuff
 * hidden within these functions.
 */

#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

namespace ccerve {
namespace sockets {

// Creating sockets
inline static int createSocket (int domain, int type, int protocol) noexcept (true) {
    return socket (domain, type, protocol);
}

// Closing sockets
inline static int closeSocket (int sock) {
    return close (sock);
}

} // namespace sockets
} // namespace ccerve
