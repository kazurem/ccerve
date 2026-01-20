#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <array>
#include "logger.h"
#include "http_parser.h"

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

        in_addr acceptConnection(int &new_socket_fd);
        void sendResponse(std::string response, const in_addr client_addr, std::string status_line);
        void createAndBindSocket();

        HTTPRequest req_handler;
        HTTPResponse response;
        Logger *logger;
    };
}
