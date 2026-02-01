#pragma once
#include <cstdio>
#include <exception>
#include <mutex>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

// Forward declaration of warn method
void warn(std::string_view p_Msg);

/* @brief Interface Class for all sinks. Sinks are objects to which we 
   will write our logs to.*/
class BaseSink {
    public:
        // @brief writes to the appropriate resource
        virtual void write(std::string_view p_Msg);
        virtual ~BaseSink() = default;
};

// @brief Stdout sink
class StdOutSink: public BaseSink {
    public:
    // @brief Writes to standard output
    virtual void write(std::string_view p_Msg) override;
    virtual ~StdOutSink() = default;
};

// @brief Sink for files.
class FileSink: public BaseSink {
    public:
        /**
        * @brief Constructor for FileSink class.
        * @param p_Path The path of the log file. If the path and file do not exist,
        * they will be created.
        */ 
        FileSink(std::string_view p_Path);

        // @brief Destructor for FileSink class. (This closes the file if is open)
        virtual ~FileSink();


        // @brief Writes to file associated with the sink
        virtual void write(std::string_view p_Msg) override;

        // @brief Vector of all paths 
        static std::vector<std::string_view> FileSinks;

    private:
        // @brief The file to which the messages will be written
        std::ofstream m_File;

        // @brief The name of the file
        std::string m_FilePath;

        // @brief Mutex to protect file writes
        std::mutex m_FileMutex;
};

// @brief Exception for when two file sinks point to the same file
class DuplicateFileSink: public std::exception {
    public:
    DuplicateFileSink(const char* p_Msg): m_Msg(p_Msg) {};

    const char* what() const noexcept override {
        return m_Msg.c_str();
    }

    private:
    std::string m_Msg;
};