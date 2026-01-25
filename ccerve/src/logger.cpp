#include "logger.hpp"
#include <chrono>
#include <ctime>

Logger::Logger(std::string file_path, bool log_to_file)
{
    this->file_path = file_path;
    this->log_to_file = log_to_file;

    current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(current_time_str, sizeof(current_time_str),
                  "%c", std::localtime(&current_time));

	if (log_to_file == true) {
      write_to_file.open(this->file_path);
      if (!write_to_file.is_open()) {
        std::cerr << "ERROR: Log file couldnt be opened! Exiting Program!"
                  << std::endl;
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
	using namespace std::chrono;

	current_time = std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now());

	std::cout << "["<< current_time_str << "] " << message << "\n";
	
	if (log_to_file == true) {
		write_to_file << std::ctime(&current_time) << message << '\n'
					<< std::flush;
	}
}
