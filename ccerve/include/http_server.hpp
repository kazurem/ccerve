#pragma once
#include <iostream>
#include <sstream>
#include <array>

// Source - https://stackoverflow.com/a
// Posted by user4200092, modified by community. See post 'Timeline' for change
// history Retrieved 2026-01-25, License - CC BY-SA 3.0

#ifdef _WIN32
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 /* Windows XP. */
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <Ws2tcpip.h>
#include <winsock.h>

#define IS_SOCK_INVALID(sock) (sock == INVALID_SOCKET)
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <arpa/inet.h>
#include <netdb.h> /* Needed for getaddrinfo() and freeaddrinfo() */
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> /* Needed for close() */

#define IS_SOCK_INVALID(sock) (sock < 0)
#define SOCKET unsigned long long
#endif



#include "http_parser.hpp"
#include "logger.hpp"

namespace http
{
    class HTTPServer
    {
    public:
        HTTPServer(std::string ip_addr, int port, std::string file_to_read = "index.html", bool log_to_file = true);
        ~HTTPServer();
        void startListeningSession();
        void stopListeningSession();

    private:
        int port;
        std::string ip_address;
        int socket_file_descriptor;
        int new_socket_file_descriptor;
        struct sockaddr_in socket_address;
        unsigned int socket_address_length;
        
        static const int BUFFER_SIZE = 30760;
        std::string server_message;

        int sockInit(void);
        int sockQuit(void);
		int closeSocket(SOCKET sock);
        void createSocket();
        void bindSocket();

        in_addr acceptConnection(int &new_socket_fd);
        void sendResponse(std::string response, const in_addr client_addr, std::string status_line);
        void createAndBindSocket();

        HTTPRequest req_handler;
        HTTPResponse response;
        Logger *logger;
    };
}
