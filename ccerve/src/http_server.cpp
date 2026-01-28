#include "http_server.hpp"
#include "http_parser.hpp"
#include "sockets.hpp"

#include <condition_variable>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>

static std::queue<std::pair<int, in_addr>> connection_queue;
static std::mutex queue_mutex;
static std::condition_variable non_empty;

static void InitLogger() {
    //File and Console Loggers
    //auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("cerve-logs/cerve-log.txt", true);
    std::vector<spdlog::sink_ptr> sinks{file_sink}; //there has to be a better way?
    auto logger = std::make_shared<spdlog::logger>("cerve_logger", begin(sinks), end(sinks));
    spdlog::set_default_logger(logger);
}

HTTPServer::HTTPServer(std::string ip_address, int port, std::string file_to_read, bool log_to_file)
{
    this->ip_address = ip_address;
    this->port = port;

    server_sock_addr_len = sizeof(server_sock_addr);
    InitLogger();

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
        spdlog::error("Socket creation failed!");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
}

void HTTPServer::bindServerSocket() {
    if (bind(server_sock_fd, (sockaddr *)&server_sock_addr, server_sock_addr_len) < 0)
    {
        std::ostringstream osstr;
        osstr << "Socket could not be bound to ADDRESS " << inet_ntoa(server_sock_addr.sin_addr) << " on PORT " << ntohs(server_sock_addr.sin_port);
        spdlog::error(osstr.str());
        
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    
}

//closing socket file descriptors
void HTTPServer::stopListeningSession()
{
    spdlog::info("Closing server!");
    closeSocket(server_sock_fd);
    closeSocket(client_sock_fd);
    exit(EXIT_SUCCESS);
}

auto HTTPServer::handleConnection(int server_sock_fd)  -> void {

    char buffer[BUFFER_SIZE] = {0};
    while(true) {
            std::pair<int, in_addr> client;

            {
                std::unique_lock l(queue_mutex);
                while (connection_queue.empty()) {
                    non_empty.wait(l);
                }

                client = connection_queue.front();
                connection_queue.pop(); 
            }

            bool keep_alive = true;
            while(keep_alive) {
                // receive request from client
                int bytes_received = recv(client.first, buffer, BUFFER_SIZE, 0);
                if (bytes_received == 0) {
                    // Client closed the connection (Normal)
                    break; 
                } else if (bytes_received < 0) {
                    spdlog::error("Socket was not able to read data\n");
                    break;
                }
                
                // handle request 
                http::HeaderMap header_map;
                std::string resp = http::handleRequest(header_map, std::string(buffer, bytes_received));

                // send response
                sendResponse(resp, client.first, server_sock_fd, client.second, (header_map["method"] + " " + header_map["resource-path"] + " " + header_map["http-version"]));

                // Check for "close" explicitly, otherwise assume keep-alive for HTTP/1.1
                if (header_map["Connection"].find("close") != std::string::npos) {
                    keep_alive = false;
                }
            }

            // close socket
            shutdown(client.first, SHUT_WR);
            closeSocket(client.first);
            // std::this_thread::sleep_for(std::chrono::seconds());
    }
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
    spdlog::info(osstr.str());

    // listen
    if (listen(server_sock_fd, 4096) < 0)
    {
        spdlog::error("Socket was not able to start listening!");
        closeSocket(server_sock_fd);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 10; i++) {
        threads.push_back(std::thread(handleConnection, server_sock_fd));
    }
    

    while (true)
    {
        auto client_addr = acceptConnection(client_sock_fd);

        std::unique_lock l(queue_mutex);
        connection_queue.push({client_sock_fd, client_addr});
        non_empty.notify_one();  // the queue is non-empty now, so wake up one of the blocked consumers (if there is one) so they can retest.
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
        spdlog::error("Socket was not able to accept the connection!");
        closeSocket(server_sock_fd);
        closeSocket(client_sock_fd);
        exit(EXIT_FAILURE);
    }

    //return the client address (for log messages later)
    return client_addr.sin_addr;
}

void HTTPServer::sendResponse(const std::string& response, int client_sock_fd, int server_sock_fd, const in_addr& client_addr, const std::string& status_line)
{
    int bytes_sent = write(client_sock_fd, response.c_str(), response.size());

    //check if the whole message was able to be sent or not
    if (static_cast<size_t>(bytes_sent) != response.size())
    {
        spdlog::error("Socket was not able to send data!");
        closeSocket(server_sock_fd);
        closeSocket(client_sock_fd);
        exit(EXIT_FAILURE);
    }

    // spdlog::info(response);
    spdlog::info(std::string(inet_ntoa(client_addr)) + " -- " + status_line + "\n");
}
