#pragma once
/**
* @file Header file containing declarations of HTTP parsing functions
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <filesystem>

#include "GLOBAL.hpp"


/**
* @brief Namespace for the HTTP specific functions
*/
namespace http {

    using HeaderMap = std::map<std::string, std::string>;

    /**
    * @brief Fills the map with key-value (header: value) pairs that are present in the HTTP message
    * @param header_map (HeaderMap&) 
    * @param message (std::string) HTTP message
    */
    auto parseMessage(HeaderMap& header_map, const std::string& message) -> void;

    /**
    * @brief  Fills the map with request headers + response headers. The response
    * headers are filled by calling appropriate functions that query for resources asked for in the request. The key difference in this function and the parseMessage() function is that this assumes that the message is a HTTP Request.
    * @param  header_map (HeaderMap&).
    * @param  message HTTP request
    * @return response (std::string) HTTP response
    */
    auto handleRequest(HeaderMap& header_map, const std::string& http_request) -> std::string;

    auto printHeaderMap(const HeaderMap& header_map) -> void;

    auto printHeaderKeys(const HeaderMap& header_map) -> void;
}
