#pragma once

/**
 * @file exception.hpp
 * @brief Contains declarations and implementations for custom exceptions.
 */

#include <exception>
#include <string>

namespace ccerve {

/**
 * @namespace Namespace for custom exceptions
 */
namespace exception {

// @brief Exception for when creation of the server socket fails
class ServerSockCreationFailure : public std::exception {
    private:
    std::string message;

    public:
    // Constructor accepting const char*
    ServerSockCreationFailure (const char* msg) : message (msg) {
    }

    const char* what () const noexcept {
        return message.c_str ();
    }
};

// @brief Exception for when the process of binding the socket with the address
// fails
class ServerSockBindFailure : public std::exception {
    private:
    std::string message;

    public:
    // Constructor accepting const char*
    ServerSockBindFailure (const char* msg) : message (msg) {
    }

    const char* what () const noexcept {
        return message.c_str ();
    }
};

} // namespace exception
} // namespace ccerve