#include "logger.hpp"

void setLogFormat(std::string_view p_NewLogFormat) {
    s_LogFormat = p_NewLogFormat;
}

std::vector<const Logger*> Logger::Loggers = std::vector<const Logger*>();

Logger::Logger(std::string_view p_LoggerName, LOG_LEVEL p_LogLevel, const SinksVector& p_Sinks) {
    m_LoggerName = p_LoggerName;
    m_LogLevel = p_LogLevel;

    m_LogQueue = std::queue<std::string>();
    m_Sinks = p_Sinks; // ! PROBLEMATIC ?
    
    Loggers.push_back(this);
    m_StopWriteThread = false;
    m_WriteThread = std::thread([this] { write(); });
}

Logger::~Logger() {
    Loggers.erase(std::remove(Loggers.begin(), Loggers.end(), this), Loggers.end());

    m_StopWriteThread = true;
    m_WriteThread.join();
}

void Logger::write() const {
    using namespace std::chrono_literals;
    while(not m_StopWriteThread or not m_LogQueue.empty()) {
        {
            std::lock_guard<std::mutex> queue_lock{m_LogQueueMutex};
            if(not m_LogQueue.empty()) {
                for(auto& sink : m_Sinks) {
                    sink->write(m_LogQueue.front()); // ! Multiple front() calls (one for each sink)
                }
                m_LogQueue.pop();
            }
        }
        // std::this_thread::sleep_for(5ms);
    }
}

void Logger::log(std::string_view p_Msg, LOG_LEVEL p_LogLevel) const {
    if(p_LogLevel < m_LogLevel) return;

    // format msg (seperate the formatting to a function pweese)
    auto time = getCurrentTime();
    auto temp_str = "[" + std::string(time) + "] -  " + "[" + m_LoggerName + "] - " + std::string(p_Msg) + "\n";

    // push to log queue
    std::lock_guard<std::mutex> queue_lock{m_LogQueueMutex};
    m_LogQueue.push(temp_str);
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

// @brief function for testing threading capabilities of logger
// void f() {
//     int i;
//     while(true) {
//         i = rand() % 1000;
//         if(i%2 == 0)
//             info(std::format("thread: {}, i: {}", std::this_thread::get_id(), i));
//     }
// }

// int main() {
//     auto file_sink = std::make_shared<FileSink>("log.txt");
//     getDefaultLogger()->addSink(file_sink);
//     auto log = new Logger("Custom Logger");
//     log->addSink(file_sink);

//     int i = 0;
//     while(i != 200) {
//         i = rand() % 1000;
//         if(i%2 == 0)
//                 log->info(std::format(""));
//     }
    
//     std::thread t1(f);
//     std::thread t2(f);
//     info("Hello World");
//     t1.join();
//     t2.join();
// }