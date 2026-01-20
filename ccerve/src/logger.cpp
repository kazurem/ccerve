#include "logger.h"

Logger::Logger(std::string file_path, bool log_to_file)
{
    this->file_path = file_path;
    this->log_to_file = log_to_file;
    if (log_to_file == true)
    {
        write_to_file.open(this->file_path);
        if (!write_to_file.is_open())
        {
            std::cerr << "ERROR: Log file couldnt be opened! Exiting Program!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

Logger::~Logger()
{
    write_to_file.close();
}

void Logger::log(std::string message)
{
    time(&current_time);
    std::cout << message << " " << ctime(&current_time);
    if (log_to_file == true)
    {
        write_to_file << message << ctime(&current_time) << '\n'
                      << std::flush;
    }
}
