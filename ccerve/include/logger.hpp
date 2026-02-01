#pragma once
#include <atomic>
#include <condition_variable>
#include <format>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "utils.hpp"
#include "colors.hpp"
#include "sinks.hpp"

/*
* - Add LOG_LEVEL functionality (DONE: added line "if(p_LogLevel < m_LogLevel) return" in Logger::log)
* - Add std::format like functionality to logging functions (DONE)
* - Add Format changing functionality (DONE)
* - Fix double (triple if main.cpp's main is run) initialization problem for Logger (DONE: I was initializing the default logger in a header file. Fix was to declare it in the source file)
* - ANSI color codes get printed to logs
* - Thread safe logging (DONE)
*/

// @brief Enum for different log level options (INFO, WARN, ERROR)
enum LOG_LEVEL {
    INFO,
    WARN,
    ERROR,
};

using SinksVector = std::vector<std::shared_ptr<BaseSink>>;

// @brief Main logger class
class Logger {

public:
    Logger(std::string_view p = "", 
           LOG_LEVEL p_Level = LOG_LEVEL::INFO, const SinksVector& p_Sinks = SinksVector({std::make_shared<StdOutSink>(StdOutSink())}));
    ~Logger();

    // @brief Pushes the sink to the sinks vector
    void addSink(std::shared_ptr<BaseSink> p_Sink);

    // @brief setter function for log level
    void setLogLevel(LOG_LEVEL p_LogLevel);

    // @brief getter function for log level
    LOG_LEVEL getLogLevel();

    // @brief setter function for logger name
    void setLoggerName(std::string_view p_LoggerName);

    // @brief getter function for logger name
    std::string_view getLoggerName();
    
    // @brief pops from @var m_LogQueue and writes it to all sinks
    void write() const;

    /**
    * @brief Pushed log message to log queue (thread safe). 
    * @param p_Msg message to push
    * @param p_LogLevel if the log level of the logger is larger than this,
    * the message won't be pushed.
    */
    template<typename ...Args>
    void log(std::string_view p_Msg, LOG_LEVEL p_LogLevel, std::string p_Tag, Args&& ...p_Args) const;

    // @brief Calls log() with green colored "info" string attached to the message
    template<typename ...Args>
    void info(std::string_view p_Msg, Args&& ...p_Args) const;

    // @brief Calls log() with yellow colored "warn" string attached to the message
    template<typename ...Args>
    void warn(std::string_view p_Msg, Args&& ...p_Args) const;

    // @brief Calls log() with red colored "error" string attached to the message
    template<typename ...Args>
    void error(std::string_view p_Msg, Args&& ...p_Args) const;

    // @briefs Returns the list of active Loggers (singletons singletons...)
    static std::vector<const Logger*>& getRegistry();

    private:
    std::string m_LoggerName;
    LOG_LEVEL m_LogLevel;

    // @brief Format for log messages 
    // This is a hack for now. I will change it later.
    static inline constexpr std::string_view m_LogFormat = "[{}] - [{}] - [{}] - {}\n";

    // @brief All the sinks that the logger will write to
    SinksVector m_Sinks;

    // @brief Queue to hold logs
    mutable std::queue<std::string> m_LogQueue;
    
    // @brief Mutex to lock operations on log queue
    mutable std::mutex m_LogQueueMutex;

    // @brief Condition variable to wake up the Write thread
    mutable std::condition_variable m_CV;

    /** @brief Thread to write messages to sinks.
    Note that creating two different file sinks for the same file will result in
    interleaved text. If you know that two loggers will print to the same file,
    Make a single file sink shared ptr and provide it to the addSink() function
    of each logger.
    */
    std::thread m_WriteThread;

    // @brief Variable to terminate write thread (atomic to avoid race conditions)
    std::atomic_bool m_StopWriteThread; 
};

// @brief Calls the default logger's info()
template<typename ...Args>
void info(std::string_view p_Msg, Args&& ...p_Args);

// @brief Calls the default logger's warn()
template<typename ...Args>
void warn(std::string_view p_Msg, Args&& ...p_Args);

// @brief Calls the default logger's error()
template<typename ...Args>
void error(std::string_view p_Msg, Args&& ...p_Args);

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

#include "logger_impl.hpp"