#include "http_server.hpp"

namespace http
{
    HTTPServer::HTTPServer(std::string ip_address, int port, std::string file_to_read, bool log_to_file)
    {
        this->ip_address = ip_address;
        this->port = port;

        server_sock_addr_len = sizeof(server_sock_addr);

        logger = new Logger("./log.txt", log_to_file);

        // initializing the sockaddr_in struct
        server_sock_addr.sin_family = AF_INET;
        server_sock_addr.sin_port = htons(this->port);
        server_sock_addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
        createServerSocket();
        bindServerSocket();
    }
    
    HTTPServer::~HTTPServer()
    {
        stopListeningSession();
    }

    void HTTPServer::createServerSocket() {
        server_sock_fd = createSocket(AF_INET, SOCK_STREAM, 0);
        if (server_sock_fd < 0)
        {
            (*logger).log("Socket creation failed!");
            close(server_sock_fd);
            exit(EXIT_FAILURE);
        }
    }

    void HTTPServer::bindServerSocket() {
        if (bind(server_sock_fd, (sockaddr *)&server_sock_addr, server_sock_addr_len) < 0)
        {
            std::ostringstream osstr;
            osstr << "Socket could not be bound to ADDRESS " << inet_ntoa(server_sock_addr.sin_addr) << " on PORT " << ntohs(server_sock_addr.sin_port);
            (*logger).log(osstr.str());
            
            close(server_sock_fd);
            exit(EXIT_FAILURE);
        }
    }
    
    //closing socket file descriptors
    void HTTPServer::stopListeningSession()
    {
        closeSocket(server_sock_fd);
        closeSocket(client_sock_fd);
        exit(EXIT_SUCCESS);
    }

    void HTTPServer::startListeningSession()
    {
        // Log server start message
        std::ostringstream osstr;
        osstr << "Starting listening session at address " << 
        inet_ntoa(server_sock_addr.sin_addr) << 
        " on port " << 
        ntohs(server_sock_addr.sin_port) << 
        " (http://" << 
        inet_ntoa(server_sock_addr.sin_addr) << 
        ":" << ntohs(server_sock_addr.sin_port) << 
        "/)";
        (*logger).log(osstr.str());

        // listen
        if (listen(server_sock_fd, 100) < 0)
        {
            (*logger).log("Socket was not able to start listening!");
            closeSocket(server_sock_fd);
            exit(EXIT_FAILURE);
        }

        int bytes_received;

        while (true)
        {
            const in_addr client_addr = acceptConnection(client_sock_fd);
            char buffer[BUFFER_SIZE] = {0};
            bytes_received = read(client_sock_fd, buffer, BUFFER_SIZE);
            if (bytes_received < 0)
            {
                (*logger).log("Socket was not able to read data!");
                closeSocket(server_sock_fd);
                closeSocket(client_sock_fd);
                exit(EXIT_FAILURE);
            }

            HeaderMap header_map;
            std::string resp = handleRequest(header_map, static_cast<std::string>(buffer));

            sendResponse(resp, client_addr, (header_map["method"] + " " + header_map["resource-path"] + " " + header_map["http-version"]));

            closeSocket(client_sock_fd);
        }
    }
    
    in_addr HTTPServer::acceptConnection(int &new_socket_fd)
    {
        //declare client addresss
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        new_socket_fd = accept(server_sock_fd, (sockaddr *)&client_addr, &client_addr_length);

        //check if connection was able to be accepted or not
        if (new_socket_fd < 0)
        {
            (*logger).log("Socket was not able to accept the connection!");
            closeSocket(server_sock_fd);
            closeSocket(client_sock_fd);
            exit(EXIT_FAILURE);
        }

        //return the client address (for log messages later)
        return client_addr.sin_addr;
    }
    
    void HTTPServer::sendResponse(const std::string& response, const in_addr& client_addr, const std::string& status_line)
    {
        int bytes_sent = write(client_sock_fd, response.c_str(), response.size());

        //check if the whole message was able to be sent or not
        if (static_cast<size_t>(bytes_sent) != response.size())
        {
            (*logger).log("Socket was not able to send data!");
            closeSocket(server_sock_fd);
            closeSocket(client_sock_fd);
            exit(EXIT_FAILURE);
        }

        (*logger).log(std::string(inet_ntoa(client_addr)) + " -- " + status_line + " -- ");
    }
}
