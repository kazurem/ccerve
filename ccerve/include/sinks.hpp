#pragma once
#include <cstdio>
#include <exception>
#include <mutex>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

/* @brief Interface Class for all sinks. Sinks are objects to which we 
   will write our logs to.

   Making this class abstract caused some problems when I tried to 
   make a vector of base sink shared_ptrs later on. These shared_ptrs
   were supposed to point to stdout sink and file sinks. The problem is
   that abstract classes can't be constructed. 

   Solutions I thought of:
   1. Just make the base class not abstract (I will lose the interface enforcement)
   2. Make another class which inherit from base class but is not abstract. Name it BaseSinkConstructible.
   Use this as the type for SinksVector. 
   */
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