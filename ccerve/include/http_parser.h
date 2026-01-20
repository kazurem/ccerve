#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem> //this is a c++17 library so the whole program wont compile on older c++ versions. (note to self: try to remove this)

#include "GLOBAL.h"
#include "utils.h"

class HTTPMessage
{
public:
    std::string message; // request for HTTPRequest and response for HTTPResponse
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;

    void printHeaders();
};

class HTTPRequest : public HTTPMessage
{
public:
    std::string method;
    std::string resource_path;
    std::string content_type;
    std::string content_type_status;

    std::map<std::string, std::string> handleRequest();

    void setRequest(std::string request);
    std::string getRequest();

private:
    void setHTTPResponseInfo(std::map<std::string, std::string> &info_for_response_construction, const int file_data_read_operation_status);
    void parseRequest();
    void getContentType();
    // read_mode: 1 for binary read and 0 for normal text read
    int getFileData(std::ios::openmode read_mode);
    int readDataOfResourcetoBody(std::map<std::string, std::string> &info_for_response_construction);
};

class HTTPResponse : public HTTPMessage
{
public:
    std::string status_code;
    std::string reason_phrase;
    
    std::string parseResponse(std::string response);
    std::string constructResponse(std::map<std::string, std::string> info_for_response_construction);
};
