#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem> //this is a c++17 library so the whole program wont compile on older c++ versions. (note to self: try to remove this)

#include "GLOBAL.hpp"

/**
----------------------------- FUNCTIONS FOR PARSING HTTP MESSAGES -----------------------------
*/

using HeaderMap = std::map<std::string, std::string>;

/**
* @brief construct a HTTP Response
* @param header_map (HeaderMap) Map consisting of keys (HTTP Headers) and values (Header values)
* @return HTTP response in the form of std::string
*/
static auto constructResponse(HeaderMap& header_map) -> std::string;

static auto parseRequest(HeaderMap& header_map, const std::string& message) -> void;

/**
* @brief Reads file data and puts it into the given variable (body) (by reference)
* @param header_map (HeaderMap) 
* @param read_mode For binary read mode or text read mode
* @return whether the operation was successful(0) or not(1).
*/
static auto getFileData(HeaderMap& header_map, std::ios::openmode read_mode) -> int;

/**
* @brief fills the content-type header of the header_map
* @param  header_map (HeaderMap).
*/
static auto getContentType(HeaderMap& header_map) -> void;

/**
* @brief fills the content-type header of the header_map
* @param  header_map (HeaderMap).
*/
static auto readResource(HeaderMap& header_map) -> int;

/**
* @brief fills the content-type header of the header_map
* @param  header_map (HeaderMap).
* @param  read_status (int)
*/
static auto fillHTTPResponseInfo(HeaderMap& header_map, int read_status) -> void;

static auto handleRequest(HeaderMap& header_map, const std::string& message) -> void;

static auto printHeaderMap(const HeaderMap& header_map) -> void;