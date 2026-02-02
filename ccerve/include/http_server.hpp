#pragma once

/**
 * @file http_server.hpp
 * @brief Holds the declaration of the HttpServer class
 */

#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>
#include <unistd.h>

#include "exception.hpp"
#include "http_parser.hpp"
#include "logger.hpp"
#include "sockets.hpp"

/**
 * @namespace Main namespace of the project
 */
namespace ccerve {

class HttpServer {
    public:
    HttpServer (std::string p_IPAddress, int p_Port, bool p_Log = true);
    ~HttpServer ();
    void startListeningSession ();
    void stopListeningSession ();

    private:
    // @brief File descriptor of server socket (listening socket)
    int m_ServerSock;

    // @brief Struct defining the socket address of server socket
    struct sockaddr_in m_ServerSockAddr;

    // @brief File descriptor of the socket created when connection is accepted
    int m_ClientSock;

    // @brief Struct defining the socket address of client socket
    struct sockaddr_in m_ClientSockAddr;
    socklen_t m_ClientSockAddrLen = sizeof (m_ClientSockAddr);

    // @brief Size of buffer which will store the message sent by client
    static const size_t BUFFER_SIZE = 30760; // ! Probably a better way to do this

    /**
    @brief Maximum number of connections which could wait in queue before before
    server starts rejecting
    */
    static const size_t BACKLOG_SIZE = 100;

    // @brief the string in which server response will be stored
    std::string response;

    // @brief Accept connection on given socket
    void acceptConnection ();

    // @brief Send a HTTP response to client
    void sendResponse (const std::string& p_Response, int p_ClientSock);

    /*
    These functions are called within the constructor and can throw
    exceptions.
    */
    void createServerSocket ();

    void bindServerSocket (); // Can throw ServerSockBindFail
};
} // namespace ccerve