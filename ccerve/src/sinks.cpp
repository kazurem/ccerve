/**
 * @file sinks.cpp
 * @brief Contains definitions for various sinks. Sinks are wrappers over
 * resources to which the logs are written.
 */

#include "sinks.hpp"

namespace ccerve {
namespace sinks {

void BaseSink::write (std::string_view p_Msg) {
}

std::vector<std::string_view> FileSink::FileSinks;

FileSink::FileSink (std::string_view p_Path) {
    using namespace std;
    filesystem::path path (p_Path);

    m_FilePath = p_Path;

    // Prevent creation of a duplicate sink
    if (std::find (FileSinks.begin (), FileSinks.end (), m_FilePath) !=
    FileSinks.end ()) {
        std::cerr << (std::format (
        "Two or more file sinks point to the same underlying file "
        "'{}'. This will result in interleaved text in the file.",
        p_Path)
        .c_str ());
    }

    // Check if path exists. If it doesn't,create it.
    if (path.has_parent_path () and not filesystem::exists (path.parent_path ())) {
        filesystem::create_directories (path.parent_path ());
    }

    m_File = ofstream (path); // create the log file
    FileSinks.push_back (m_FilePath);
}

FileSink::~FileSink () {
    if (m_File.is_open ())
        m_File.close (); // ! what if close() fails?
    FileSinks.erase (std::find (FileSinks.begin (), FileSinks.end (), this->m_FilePath));
}

void FileSink::write (std::string_view p_Msg) {
    std::lock_guard<std::mutex> file_lock (
    m_FileMutex); // ! Improve: Only lock when two sinks point to the same
                  // file
    m_File << p_Msg << std::flush;
}

void StdOutSink::write (std::string_view p_Msg) {
    std::cout << p_Msg;
}

} // namespace sinks
} // namespace ccerve
