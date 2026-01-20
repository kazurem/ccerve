#pragma once
#include <iostream>

static const int TOTAL_CONTENT_TYPES = 7;

static const std::string TEXT_FILE_FORMATS[TOTAL_CONTENT_TYPES] = {".html", ".css", ".svg"};
static const std::string BINARY_FILE_FORMATS[TOTAL_CONTENT_TYPES] = {".png", ".jpg", "jpeg", ".webp"};

static const std::string TEXT_CONTENT_TYPES[TOTAL_CONTENT_TYPES] = {"text/html", "text/css", "image/svg+xml"};
static const std::string IMAGE_CONTENT_TYPES[TOTAL_CONTENT_TYPES] = {"image/jpeg", "image/jpg", "image/png", "image/webp"};
static const std::string ALL_CONTENT_TYPES[TOTAL_CONTENT_TYPES] = {"text/html", "text/css", "image/svg+xml", "image/jpeg", "image/jpg", "image/png", "image/webp"};


//multiline string literals are a part of c++11 and onwards.
static const std::string CONTENT_TYPE_NOT_SUPPORTED_HTML = R"""(
                                                <!DOCTYPE html>
                                                <html>
                                                <head>
                                                    <title>404 Not Found</title>
                                                </head>
                                                <body>
                                                    <h1>Content type not supported</h1>
                                                </body>
                                                </html>)""";

static const std::string RESOURCE_NOT_FOUND_HTML = R"""(
                                                <!DOCTYPE html>
                                                <html>
                                                <head>
                                                    <title>404 Not Found</title>
                                                </head>
                                                <body>
                                                    <h1>Not Found</h1>
                                                    <p>The requested URL was not found on this server.</p>
                                                </body>
                                                </html>)""";