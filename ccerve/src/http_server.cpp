#include "http_server.h"

namespace http
{
    HTTPServer::HTTPServer(std::string ip_address, int port, std::string file_to_read, bool log_to_file)
    {
        this->ip_address = ip_address;
        this->port = port;

        socket_address_length = sizeof(socket_address);

        logger = new Logger("./log.txt", log_to_file);

        // initializing the sockaddr_in struct
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(this->port);
        socket_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
        createAndBindSocket();
    }
    
    HTTPServer::~HTTPServer()
    {
        stopListeningSession();
    }

    //creates and binds socket
    void HTTPServer::createAndBindSocket()
    {
        // creating the socket
        socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_file_descriptor < 0)
        {
            (*logger).log("Socket creation failed!");
            close(socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        // bind the socket to the address
        if (bind(socket_file_descriptor, (sockaddr *)&socket_address, socket_address_length) < 0)
        {
            std::ostringstream osstr;
            osstr << "Socket could not be bound to ADDRESS " << inet_ntoa(socket_address.sin_addr) << " on PORT " << ntohs(socket_address.sin_port);
            (*logger).log(osstr.str());

            close(socket_file_descriptor);
            exit(EXIT_FAILURE);
        }
    }
    
    //closing socket file descriptors
    void HTTPServer::stopListeningSession()
    {
        close(socket_file_descriptor);
        close(new_socket_file_descriptor);
        exit(EXIT_SUCCESS);
    }

    void HTTPServer::startListeningSession()
    {
        // Log server start message
        std::ostringstream osstr;
        osstr << "Starting listening session at address " << inet_ntoa(socket_address.sin_addr) << " on port " << ntohs(socket_address.sin_port) << " (http://" << inet_ntoa(socket_address.sin_addr) << ":" << ntohs(socket_address.sin_port) << "/)";
        (*logger).log(osstr.str());

        // listen
        if (listen(socket_file_descriptor, 100) < 0)
        {
            (*logger).log("Socket was not able to start listening!");
            close(socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        int bytes_received;

        while (true)
        {
            const in_addr client_addr = acceptConnection(new_socket_file_descriptor);
            char buffer[BUFFER_SIZE] = {0};
            bytes_received = read(new_socket_file_descriptor, buffer, BUFFER_SIZE);
            if (bytes_received < 0)
            {
                (*logger).log("Socket was not able to read data!");
                close(socket_file_descriptor);
                close(new_socket_file_descriptor);
                exit(EXIT_FAILURE);
            }

            // Send message to HTTP request handler
            req_handler.setRequest((std::string) buffer);
            std::map<std::string, std::string> http_response_info = req_handler.handleRequest();

            std::string resp = response.constructResponse(http_response_info);

            sendResponse(resp, client_addr, (req_handler.method + " " + req_handler.resource_path + " " + req_handler.http_version));

            close(new_socket_file_descriptor);
        }
    }
    
    in_addr HTTPServer::acceptConnection(int &new_socket_fd)
    {
        //declare client addresss
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        new_socket_fd = accept(socket_file_descriptor, (sockaddr *)&client_addr, &client_addr_length);

        //check if connection was able to be accepted or not
        if (new_socket_fd < 0)
        {
            (*logger).log("Socket was not able to accept the connection!");
            close(socket_file_descriptor);
            close(new_socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        //return the client address (for log messages later)
        return client_addr.sin_addr;
    }
    
    void HTTPServer::sendResponse(std::string response, in_addr client_addr, std::string status_line)
    {
        //this is the response built by the buildRespons method of the HTTPResponse class
        server_message = response;

        int bytes_sent = write(new_socket_file_descriptor, server_message.c_str(), server_message.size());

        //check if the whole message was able to be sent or not
        if ((size_t)bytes_sent != server_message.size())
        {
            (*logger).log("Socket was not able to send data!");
            close(socket_file_descriptor);
            close(new_socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        (*logger).log(std::string(inet_ntoa(client_addr)) + " -- " + status_line + " -- ");
    }
}