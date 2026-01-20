#pragma once
#include <iostream>
#include <ctime>
#include <fstream>

class Logger
{
public:
    std::string file_path;

    Logger(std::string file_path, bool log_to_file = false);
    ~Logger();
    void log(std::string message);

private:
    time_t current_time;
    std::ofstream write_to_file;
    bool log_to_file;
};