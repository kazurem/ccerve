#pragma once
#include <iostream>
#include <chrono>
#include <fstream>

#define _CRT_SECURE_NO_WARNINGS 1

class Logger
{
public:
    std::string file_path;

    Logger(std::string file_path, bool log_to_file = false);
    ~Logger();
    void log(std::string message);

private:
    time_t current_time;
	char current_time_str[100];
    std::ofstream write_to_file;
    bool log_to_file;
};
