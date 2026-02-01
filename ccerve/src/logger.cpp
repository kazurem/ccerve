#include "logger.hpp"
#include <memory>

void setLogFormat(std::string_view p_NewLogFormat) {
    s_LogFormat = p_NewLogFormat;
}

std::vector<const Logger*> Logger::Loggers = std::vector<const Logger*>();

void Logger::ListLoggers() {
    for(const auto logger : Loggers) {
        logger->info(logger->m_LoggerName);
    }
    std::cout << "\n";
}

Logger::Logger(std::string_view p_LoggerName, LOG_LEVEL p_LogLevel, const SinksVector& p_Sinks) {
    m_LoggerName = p_LoggerName;
    m_LogLevel = p_LogLevel;

    m_Sinks = p_Sinks; // ! PROBLEMATIC ?

    Loggers.push_back(this);
}

Logger::~Logger() {
    Loggers.erase(std::remove(Loggers.begin(), Loggers.end(), this), Loggers.end());
}

void Logger::log(std::string_view p_Msg, LOG_LEVEL p_LogLevel) const {
    if(p_LogLevel < m_LogLevel) return;
    auto time = getCurrentTime();
    auto temp_str = "[" + std::string(time) + "] -  " + "[" + m_LoggerName + "] - " + std::string(p_Msg) + "\n";
    for(auto& sink : m_Sinks) {
        sink->write(temp_str);
    }
}

using cpp_colors::foreground::green;
using cpp_colors::foreground::yellow;
using cpp_colors::foreground::red;
using cpp_colors::style::reset;

#define GREEN(p_Msg) { std::format("{}{}{}", green, p_Msg, reset) }
#define YELLOW(p_Msg) { std::format("{}{}{}", yellow, p_Msg, reset) }
#define RED(p_Msg) { std::format("{}{}{}", red, p_Msg, reset) }

void Logger::info(std::string_view p_Msg) const {
    auto temp_str = "[" + std::string(GREEN("info")) + "] - ";
    log( temp_str + std::string(p_Msg), LOG_LEVEL::INFO);
}

void Logger::warn(std::string_view p_Msg) const {
    auto temp_str = "[" + std::string(YELLOW("warn")) + "] - ";
    log(temp_str + std::string(p_Msg), LOG_LEVEL::WARN);
}

void Logger::error(std::string_view p_Msg) const {
    auto temp_str = "[" + std::string(RED("error")) + "] - ";
    log(temp_str + std::string(p_Msg), LOG_LEVEL::ERROR);
}

void Logger::addSink(std::shared_ptr<BaseSink> p_Sink) {
    m_Sinks.push_back(p_Sink);
}

void Logger::removeSink(std::string_view p_SinkName) {
    /* Find and remove sink with a given name (NOTE: add names to sinks?)*/
}


void Logger::setLogLevel(LOG_LEVEL p_LogLevel) {
    m_LogLevel = p_LogLevel;
}

LOG_LEVEL Logger::getLogLevel() {
    return m_LogLevel;
};

void Logger::setLoggerName(std::string_view p_LoggerName) {
    m_LoggerName = p_LoggerName;
};

std::string_view Logger::getLoggerName() {
    return m_LoggerName;
};

// @brief A logger is created by default
static std::shared_ptr<Logger> s_DefaultLogger = std::make_shared<Logger>("Default Logger");


void setDefaultLogger(Logger* p_Logger) { 
    s_DefaultLogger.reset(p_Logger);
}

std::shared_ptr<Logger> getDefaultLogger() { 
    return s_DefaultLogger;
}

void info(std::string_view p_Msg) {
    s_DefaultLogger->info(p_Msg);
}

void warn(std::string_view p_Msg) {
    s_DefaultLogger->warn(p_Msg);
}

void error(std::string_view p_Msg) {
    s_DefaultLogger->error(p_Msg);
}


int main() {
    info("Hello World");
}