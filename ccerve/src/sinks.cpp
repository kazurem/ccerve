#include "sinks.hpp"

void BaseSink::write(std::string_view p_Msg) {}

FileSink::FileSink(std::string_view p_Path) {
    using namespace std;
    filesystem::path path(p_Path);
    
    // Check if path exists. If it doesn't,create it.
    if(not filesystem::exists(path)) {
        filesystem::create_directories(path.parent_path());
    }
    m_File = ofstream(path); // create the log file
}

FileSink::~FileSink() {
    if(m_File.is_open()) m_File.close(); // ! what if close() fails?
}

void FileSink::write(std::string_view p_Msg) {
    m_File << p_Msg << std::flush;
}

void StdOutSink::write(std::string_view p_Msg) {
    std::cout << p_Msg;
}