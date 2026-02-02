/**
 * @file http_server.cpp
 * @brief Holds the definition of the HttpServer class
 */

#include "http_server.hpp"
#include "http_parser.hpp"
#include "sockets.hpp"

namespace ccerve {

HttpServer::HttpServer (std::string p_IPAddress, int p_Port, bool p_Log) {
    // Adding file sink to default logger
    auto file_sink = std::make_shared<sinks::FileSink> ("log/log.txt");
    log::getDefaultLogger ()->addSink (file_sink);

    // initializing the sockaddr_in struct
    m_ServerSockAddr.sin_family      = AF_INET;
    m_ServerSockAddr.sin_port        = htons (p_Port);
    m_ServerSockAddr.sin_addr.s_addr = inet_addr (p_IPAddress.c_str ());

    createServerSocket ();

    // Temporary (this is so that I can use the address immediatly after Ctrl+C
    // type exit)
    const int enable = 1;
    if (setsockopt (m_ServerSock, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR,
        &enable, sizeof (int)) < 0) {
        log::warn (
        "SO_REUSEPORT | SO_REUSADDR couldn't be set as an option for "
        "server socket.");
    }

    bindServerSocket ();
}

HttpServer::~HttpServer () {
    stopListeningSession ();
}

void HttpServer::createServerSocket () {
    m_ServerSock = sockets::createSocket (AF_INET, SOCK_STREAM, 0);
    if (m_ServerSock < 0) {
        throw exception::ServerSockCreationFailure (
        "Server socket creation failed!");
    }
}

void HttpServer::bindServerSocket () {
    if (bind (m_ServerSock, (sockaddr*)&m_ServerSockAddr, sizeof (m_ServerSockAddr)) < 0) {
        sockets::closeSocket (m_ServerSock);
        throw exception::ServerSockBindFailure (
        "Server socket could not be bound to the given address.");
    }
}

// closing socket file descriptors
void HttpServer::stopListeningSession () {
    /* Variable to end listening while loop */
    /* Join all threads */
}

void HttpServer::startListeningSession () {
    // listen
    if (listen (m_ServerSock, BACKLOG_SIZE) < 0) {
        log::error ("Socket was not able to start listening!");
        sockets::closeSocket (m_ServerSock);
    }

    // Log server start message
    log::info ("Starting listening session at ADDRESS {} on PORT {}",
    inet_ntoa (m_ServerSockAddr.sin_addr), ntohs (m_ServerSockAddr.sin_port));

    char buffer[BUFFER_SIZE] = { 0 };
    ssize_t bytes_received   = 0;
    while (true) {
        acceptConnection ();

        bool keep_alive = true;
        while (m_ClientSock > 0 && keep_alive) {
            // receive request from client
            bytes_received = recv (m_ClientSock, buffer, BUFFER_SIZE, 0);

            if (bytes_received == 0) {
                // Client closed the connection (Normal)
                break;
            } else if (bytes_received < 0) {
                log::error ("Socket was not able to read data\n");
                break;
            }

            // handle request
            parse::HeaderMap header_map;
            std::string resp =
            parse::handleRequest (header_map, std::string (buffer, bytes_received));

            // send response to client
            sendResponse (resp, m_ClientSock);

            log::info ("{} -- {} {} {} {}", inet_ntoa (m_ClientSockAddr.sin_addr),
            header_map["method"], header_map["resource-path"],
            header_map["http-version"], header_map["status-code"]);

            // Check for "close" explicitly, otherwise assume keep-alive for
            // HTTP/1.1
            if (header_map["Connection"].find ("close") != std::string::npos) {
                keep_alive = false;
            }
        }

        shutdown (m_ClientSock, SHUT_WR);
        sockets::closeSocket (m_ClientSock);
        memset (buffer, 0,
        bytes_received); // clear buffer before next connection
    }
}

void HttpServer::acceptConnection () {
    m_ClientSock = accept (m_ServerSock, (sockaddr*)&m_ClientSockAddr, &m_ClientSockAddrLen);

    // check if connection was able to be accepted or not
    if (m_ClientSock < 0) {
        log::error ("Socket was not able to accept the connection!");
    }
}

void HttpServer::sendResponse (const std::string& p_Response, int p_ClientSock) {
    int bytes_sent = write (p_ClientSock, p_Response.c_str (), p_Response.size ());

    // check if the whole message was able to be sent or not
    if (static_cast<size_t> (bytes_sent) != p_Response.size ()) {
        log::error ("Socket was not able to send data!");
    }
}

} // namespace ccerve
