#include "http_server.hpp"
#include <string>
#include <winsock2.h>

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

		sockInit();

        createSocket();
		bindSocket();
    }
    
    HTTPServer::~HTTPServer()
    {
        stopListeningSession();
    }

    // Source - https://stackoverflow.com/a
    // Posted by user4200092, modified by community. See post 'Timeline' for
    // change history Retrieved 2026-01-25, License - CC BY-SA 3.0

	int HTTPServer::sockInit(void) {
		#ifdef _WIN32
			WSADATA wsa_data;
			return WSAStartup(MAKEWORD(1, 1), &wsa_data);
		#else
			return 0;
		#endif
	}

	int HTTPServer::sockQuit(void) {
		#ifdef _WIN32
			return WSACleanup();
		#else
			return 0;
		#endif
    }

    int HTTPServer::closeSocket(SOCKET sock) {
		int status = 0;

		#ifdef _WIN32
		status = shutdown(sock, SD_BOTH);
		if (status == 0) {
			status = closesocket(sock);
		}
		#else
			status = shutdown(sock, SHUT_RDWR);
			if (status == 0) {
				status = close(sock);
			}
		#endif

		return status;
	}

	void HTTPServer::createSocket() {
		socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (IS_SOCK_INVALID(socket_file_descriptor)) {
			(*logger).log("Socket creation failed!");
			closeSocket(socket_file_descriptor);
			exit(EXIT_FAILURE);
		}
    }

	void HTTPServer::bindSocket() {
		if (bind(socket_file_descriptor, (sockaddr *)&socket_address,
				socket_address_length) < 0) {
			std::ostringstream osstr;
			osstr << "Socket could not be bound to ADDRESS "
					<< inet_ntoa(socket_address.sin_addr) << " on PORT "
					<< ntohs(socket_address.sin_port);
			(*logger).log(osstr.str());

			closeSocket(socket_file_descriptor);
			exit(EXIT_FAILURE);
		}
    }

    //closing socket file descriptors
    void HTTPServer::stopListeningSession()
    {
        closeSocket(socket_file_descriptor);
        closeSocket(new_socket_file_descriptor);
		sockQuit();
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
            closeSocket(socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        int bytes_received;

        while (true)
        {
            const in_addr client_addr = acceptConnection(new_socket_file_descriptor);
            char buffer[BUFFER_SIZE] = {0};
            bytes_received = recv(new_socket_file_descriptor, buffer, BUFFER_SIZE, 0);
            if (bytes_received < 0)
            {
                (*logger).log("Socket was not able to read data!");
                closeSocket(socket_file_descriptor);
                closeSocket(new_socket_file_descriptor);
                exit(EXIT_FAILURE);
            }

            // Send message to HTTP request handler
            req_handler.setRequest((std::string) buffer);
            std::map<std::string, std::string> http_response_info = req_handler.handleRequest();

            std::string resp = response.constructResponse(http_response_info);

            sendResponse(resp, client_addr, (req_handler.method + " " + req_handler.resource_path + " " + req_handler.http_version));

            closeSocket(new_socket_file_descriptor);
        }
    }
    
    in_addr HTTPServer::acceptConnection(int &new_socket_fd)
    {
        //declare client addresss
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);

        new_socket_fd = accept(socket_file_descriptor, (sockaddr *)&client_addr, &client_addr_length);

        //check if connection was able to be accepted or not
        if (IS_SOCK_INVALID(new_socket_fd)) {
          (*logger).log("Socket was not able to accept the connection!");
          closeSocket(socket_file_descriptor);
          closeSocket(new_socket_file_descriptor);
          exit(EXIT_FAILURE);
        }

        //return the client address (for log messages later)
        return client_addr.sin_addr;
    }
    
    void HTTPServer::sendResponse(std::string response, in_addr client_addr, std::string status_line)
    {
        //this is the response built by the buildRespons method of the HTTPResponse class
        server_message = response;

        int bytes_sent = send(new_socket_file_descriptor, server_message.c_str(), server_message.size(), 0);

        //check if the whole message was able to be sent or not
        if ((size_t)bytes_sent != server_message.size())
        {
            (*logger).log("Socket was not able to send data!");
            closeSocket(socket_file_descriptor);
            closeSocket(new_socket_file_descriptor);
            exit(EXIT_FAILURE);
        }

        (*logger).log(std::string(inet_ntoa(client_addr)) + " -- " + status_line + " -- ");
    }
}
