#include "http_parser.h"

void HTTPMessage::printHeaders()
{
    std::cout << "Headers:" << std::endl;
    for (const auto &header : headers)
    {
        std::cout << header.first << ": " << header.second << std::endl;
    }
}

void HTTPRequest::setRequest(std::string request)
{
    this->message = request;
}

std::string HTTPRequest::getRequest()
{
    return this->message;
}

void HTTPRequest::parseRequest()
{
    std::istringstream request_message_stream(message);

    request_message_stream >> method >> resource_path >> http_version;
    resource_path = "." + resource_path; // resource_path by default is "/x/y/..../z.ext". convert it to "./x/y/.../z.ext"

    // serve index.html if no resource is specified
    if (resource_path == "./" || resource_path == "")
    {
        resource_path = "./index.html";
        content_type = "text/html";
    }

    std::string header_line;

    while (std::getline(request_message_stream, header_line))
    {
        // Ignore empty lines (end of headers)
        if (header_line.empty())
            continue;

        // Find the position of the first colon
        size_t colon_pos = header_line.find(':');
        if (colon_pos != std::string::npos)
        {
            std::string header_name = header_line.substr(0, colon_pos);
            std::string header_value = header_line.substr(colon_pos + 1);

            // Trim leading spaces in header value
            header_value.erase(0, header_value.find_first_not_of(" \t"));

            // Insert into map
            headers[header_name] = header_value;
        }
    }
}

//(note to self: write a test for this)
// returns the content type of the requested resource. Such as, text/html, text/css, image/png etc
void HTTPRequest::getContentType()
{
    std::filesystem::path resrc_path(resource_path);
    content_type = resrc_path.extension().string();

    size_t position_of_extension_dot = content_type.find(".");

    if (position_of_extension_dot != std::string::npos)
    {
        for (int i = 0; i < TOTAL_CONTENT_TYPES; i++)
        {
            if (content_type == TEXT_FILE_FORMATS[i])
            {
                // resource_extension has the format ".ext". I want ext
                // used .substr(position_of_dot + 1) to return everything after the "."
                content_type = "text/" + content_type.substr(position_of_extension_dot + 1);
                return;
            }
            else if (content_type == BINARY_FILE_FORMATS[i])
            {
                content_type = "image/" + content_type.substr(position_of_extension_dot + 1);
                return;
            }
        }
        content_type = "USCT";
    }
}

int HTTPRequest::getFileData(std::ios::openmode read_mode)
{
    std::ifstream file(resource_path, read_mode);

    if (std::filesystem::is_directory(resource_path))
    {
        return EXIT_FAILURE;
    }

    // read all data and store it in body variable(class member)
    if (file.is_open())
    {
        std::ostringstream osstr;
        osstr << file.rdbuf();

        body = osstr.str();

        file.close();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

std::map<std::string, std::string> HTTPRequest::handleRequest()
{
    parseRequest();
    getContentType();

    // info required to build http response
    std::map<std::string, std::string> info_for_response_construction;

    for (int i = 0; i < TOTAL_CONTENT_TYPES; i++)
    {
        if (method == "GET")
        {
            if (content_type == ALL_CONTENT_TYPES[i])
            {
                int file_data_read_operation_status = readDataOfResourcetoBody(info_for_response_construction);
                setHTTPResponseInfo(info_for_response_construction, file_data_read_operation_status);
                return info_for_response_construction;
            }
        }
        //could add further else if statements to incorporate other HTTP methods like PUT, POST
    }
    setHTTPResponseInfo(info_for_response_construction, EXIT_SUCCESS);
    return info_for_response_construction;
}

int HTTPRequest::readDataOfResourcetoBody(std::map<std::string, std::string> &info_for_response_construction)
{
    int file_data_read_operation_status;

    // read file data in the appropriate manner according to their extensions e.g binary read for images
    for (int i = 0; i < TOTAL_CONTENT_TYPES; i++)
    {
        if (content_type == TEXT_CONTENT_TYPES[i])
        {
            file_data_read_operation_status = getFileData(std::ios::in); // read file in normal text mode
        }
        else if (content_type == IMAGE_CONTENT_TYPES[i])
        {
            file_data_read_operation_status = getFileData(std::ios::binary); // read file in binary mode
        }
    }
    return file_data_read_operation_status;
}

void HTTPRequest::setHTTPResponseInfo(std::map<std::string, std::string> &info_for_response_construction, const int file_data_read_operation_status)
{
    info_for_response_construction["http-version"] = http_version;

    if (file_data_read_operation_status == EXIT_FAILURE)
    {
        info_for_response_construction["status-code"] = "404";
        info_for_response_construction["reason-phrase"] = "Not Found";
        info_for_response_construction["body"] = RESOURCE_NOT_FOUND_HTML;
    }
    else
    {
        if (content_type == "USCT" || content_type == "")
        {
            info_for_response_construction["status-code"] = "404";
            info_for_response_construction["reason-phrase"] = "Bad Request";
            info_for_response_construction["content-type"] = "Content-Type: " + content_type;
            info_for_response_construction["body"] = CONTENT_TYPE_NOT_SUPPORTED_HTML;
        }
        else
        {
            info_for_response_construction["status-code"] = "200";
            info_for_response_construction["reason-phrase"] = "OK";
            info_for_response_construction["content-type"] = "Content-Type: " + content_type;
            info_for_response_construction["body"] = body;
        }
    }
}

std::string HTTPResponse::constructResponse(std::map<std::string, std::string> info_for_response_construction)
{
    status_code = info_for_response_construction["status-code"];
    reason_phrase = info_for_response_construction["reason-phrase"];
    http_version = info_for_response_construction["http-version"];

    std::string response = "";
    response += info_for_response_construction["http-version"] + " ";
    response += info_for_response_construction["status-code"] + " ";
    response += info_for_response_construction["reason-phrase"] + "\r\n";
    response += info_for_response_construction["content-type"] + "\r\n\r\n";

    response += info_for_response_construction["body"];

    return response;
}
