#pragma once
#include <iostream>
#include <sstream>
#include <array>

#include "sockets.hpp"
#include "logger.hpp"
#include "http_parser.hpp"

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
        int server_sock_fd;
        int client_sock_fd;
        struct sockaddr_in server_sock_addr;
        unsigned int server_sock_addr_len;
        
        static const int BUFFER_SIZE = 30760;
        std::string response;

        in_addr acceptConnection(int &new_socket_fd);
        void sendResponse(const std::string& response, const in_addr& client_addr, const std::string& status_line);
        void createServerSocket();
        void bindServerSocket();

        Logger *logger;
    };
}
