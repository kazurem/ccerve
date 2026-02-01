#pragma once
#include <string_view>
#include <fstream>
#include <filesystem>
#include <iostream>

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

    private:
        // @brief The file to which the messages will be written
        std::ofstream m_File;
};
