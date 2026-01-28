#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "http_parser.hpp"
#include "logger.hpp"
#include "sockets.hpp"

class HTTPServer {
  public:
    HTTPServer(std::string ip_addr, int port,
               std::string file_to_read = "index.html",
               bool        log_to_file  = false);
    ~HTTPServer();
    auto startListeningSession() -> void;
    auto stopListeningSession() -> void;
    static std::shared_ptr<spdlog::logger> logger;

  private:
    int                port;
    std::string        ip_address;
    int                server_sock_fd;
    int                client_sock_fd;
    struct sockaddr_in server_sock_addr;
    unsigned int       server_sock_addr_len;

    std::vector<std::thread> threads;

    static const int BUFFER_SIZE = 30760;
    std::string      response;

    auto acceptConnection(int &new_socket_fd) -> in_addr;
    auto createServerSocket() -> void;
    auto bindServerSocket() -> void;

    
    static auto sendResponse(const std::string &response, int client_sock_fd,
                             int server_sock_fd, const in_addr &client_addr,
                             const std::string &status_line) -> void;
    static auto handleConnection(int server_sock_fd) -> void;
};
