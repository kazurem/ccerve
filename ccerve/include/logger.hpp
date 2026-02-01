#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "utils.hpp"
#include "colors.hpp"
#include "sinks.hpp"

/*
* - Add LOG_LEVEL functionality (DONE: added line "if(p_LogLevel < m_LogLevel) return" in Logger::log)
* - Add std::format like functionality to logging functions
* - Add Format changing functionality
* - Fix double (triple if main.cpp's main is run) initialization problem for Logger (DONE: I was initializing the default logger in a header file. Fix was to declare it in the source file)
* - ANSI color codes get printed to logs
* - Thread safe logging
*/

// @brief Enum for different log level options (INFO, WARN, ERROR)
enum LOG_LEVEL {
    INFO,
    WARN,
    ERROR,
};

// @brief Format for log messages
static std::string s_LogFormat = "[%t] - [%l] - [%ll] - %m";

// @brief Sets the log format to be the given format
void setLogFormat(std::string_view p_NewLogFormat);

using SinksVector = std::vector<std::shared_ptr<BaseSink>>;

// @brief Main logger class
class Logger {
public:
    Logger(std::string_view p_LoggerName = "Logger" + std::to_string(Loggers.size()), 
           LOG_LEVEL p_Level = LOG_LEVEL::INFO, const SinksVector& p_Sinks = SinksVector({std::make_shared<StdOutSink>(StdOutSink())}));
    ~Logger();

    // @brief Pushes the sink to the sinks vector
    void addSink(std::shared_ptr<BaseSink> p_Sink);

    // @brief Prints out all the sinks to standard out
    void listSinks() const;

    // @brief Removes sink with the given name from the sinks vector
    void removeSink(std::string_view p_SinkName);


    // @brief setter function for log level
    void setLogLevel(LOG_LEVEL p_LogLevel);

    // @brief getter function for log level
    LOG_LEVEL getLogLevel();

    // @brief setter function for logger name
    void setLoggerName(std::string_view p_LoggerName);

    // @brief getter function for logger name
    std::string_view getLoggerName();
    

    /**
    * @brief Write log message to all sinks. 
    * @param p_Msg message to write
    * @param p_LogLevel if the log level of the logger is larger than this,
    * the message won't be written.
    */
    void log(std::string_view p_Msg, LOG_LEVEL p_LogLevel) const;

    // @brief Calls log() with green colored "info" string attached to the message
    void info(std::string_view p_Msg) const;

    // @brief Calls log() with yellow colored "warn" string attached to the message
    void warn(std::string_view p_Msg) const;

    // @brief Calls log() with red colored "error" string attached to the message
    void error(std::string_view p_Msg) const;

    // @brief List of loggers
    static std::vector<const Logger*> Loggers; // ! PROBLEMATIC?

    // @brief Lists all the active loggers
    static void ListLoggers();

    private:
    // @brief All the sinks that the logger will write to
    SinksVector m_Sinks;

    std::string m_LoggerName;
    LOG_LEVEL m_LogLevel;
};

// @brief Calls the default logger's info()
void info(std::string_view p_Msg);

// @brief Calls the default logger's warn()
void warn(std::string_view p_Msg);

// @brief Calls the default logger's error()
void error(std::string_view p_Msg);

/** 
* @brief Changes the default logger to the one given as argument
* @param p_Logger Pointer to the logger created in the heap. DO NOT give 
pointer to a logger allocated on the stack.
*/
void setDefaultLogger(Logger* p_Logger);

/**
* @brief Gets the default logger
* @return shared_ptr to the default logger
*/
std::shared_ptr<Logger> getDefaultLogger();