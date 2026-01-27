#include "http_parser.hpp"

auto constructResponse(HeaderMap& header_map) -> std::string {
    std::string response = "";
    response += header_map["http-version"] + " ";
    response += header_map["status-code"] + " ";
    response += header_map["reason-phrase"] + "\r\n";
    response += header_map["content-type"] + "\r\n\r\n";

    response += header_map["body"];

    return response;
}

auto parseRequest(HeaderMap& header_map, const std::string& message) -> void
{
    std::istringstream request_message_stream(message);

    request_message_stream >> header_map["method"] >> header_map["resource-path"] >> header_map["http-version"];
    header_map["resource-path"] = "." + header_map["resource-path"]; // resource_path by default is "/x/y/..../z.ext". convert it to "./x/y/.../z.ext"

    // serve index.html if no resource is specified
    if (header_map["resource-path"] == "./" || header_map["resource-path"] == "")
    {
        header_map["resource-path"] = "./index.html";
        header_map["content-type"] = "text/html";
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
            header_map[header_name] = header_value;
        }
    }
}

auto getFileData(HeaderMap& header_map, std::ios::openmode read_mode) -> int
{
    std::ifstream file(header_map["resource-path"], read_mode);

    if (std::filesystem::is_directory(header_map["resource-path"]))
    {
        return EXIT_FAILURE;
    }

    // read all data and store it in body variable
    if (file.is_open())
    {
        std::ostringstream osstr;
        osstr << file.rdbuf();

        header_map["body"] = osstr.str();

        file.close();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

void getContentType(HeaderMap& header_map)
{
    std::filesystem::path resrc_path(header_map["resource-path"]);
    header_map["content-type"] = resrc_path.extension().string();

    size_t position_of_extension_dot = header_map["content-type"].find(".");

    std::string temp_content_type = header_map["content-type"];
    if (position_of_extension_dot != std::string::npos)
    {
        for (int i = 0; i < TOTAL_CONTENT_TYPES; i++)
        {
            if (temp_content_type == TEXT_FILE_FORMATS[i])
            {
                // resource_extension has the format ".ext". I want ext
                // used .substr(position_of_dot + 1) to return everything after the "."
                temp_content_type = "text/" + temp_content_type.substr(position_of_extension_dot + 1);
                header_map["content-type"] = temp_content_type;
                return;
            }
            else if (temp_content_type == BINARY_FILE_FORMATS[i])
            {
                temp_content_type = "image/" + temp_content_type.substr(position_of_extension_dot + 1);
                header_map["content-type"] = temp_content_type;
                return;
            }
        }
        
        temp_content_type = "USCT";
        header_map["content-type"] = temp_content_type;
    }
}

int readResource(HeaderMap& header_map)
{
    int read_status;

    // read file data in the appropriate manner according to their extensions e.g binary read for images
    for (int i = 0; i < TOTAL_CONTENT_TYPES; i++)
    {
        if (header_map["content-type"] == TEXT_CONTENT_TYPES[i])
        {
            read_status = getFileData(header_map, std::ios::in); // read file in normal text mode
        }
        else if (header_map["content-type"] == IMAGE_CONTENT_TYPES[i])
        {
            read_status = getFileData(header_map, std::ios::binary); // read file in binary mode
        }
    }
    return read_status;
}

auto fillHTTPResponseInfo(HeaderMap& header_map, int read_status) -> void
{
    if (read_status == EXIT_FAILURE)
    {
        header_map["status-code"] = "404";
        header_map["reason-phrase"] = "Not Found";
        header_map["body"] = RESOURCE_NOT_FOUND_HTML;
    }
    else
    {
        if (header_map["content-type"] == "USCT" || header_map["content-type"] == "")
        {
            header_map["status-code"] = "404";
            header_map["reason-phrase"] = "Bad Request";
            header_map["body"] = CONTENT_TYPE_NOT_SUPPORTED_HTML;
        }
        else
        {
            header_map["status-code"] = "200";
            header_map["reason-phrase"] = "OK";
        }
    }
}

auto handleRequest(HeaderMap& header_map, const std::string& message) -> void
{
    parseRequest(header_map, message);
    getContentType(header_map);

    for (int i = 0; i < TOTAL_CONTENT_TYPES; i++) // ? move the content_type check into for loop?
    {
        if (header_map["method"] == "GET")
        {
            if (header_map["content-type"] == ALL_CONTENT_TYPES[i])
            {
                int file_data_read_operation_status = readResource(header_map);
                fillHTTPResponseInfo(header_map, file_data_read_operation_status);
            }
        }
        //could add further else if statements to incorporate other HTTP methods like PUT, POST
    }
    fillHTTPResponseInfo(header_map, EXIT_SUCCESS);
}

auto printHeaderMap(const HeaderMap& header_map) -> void {
    for(const auto& [key, value] : header_map) {
        std::cout << key << ": " << value << "\n";
    }
}