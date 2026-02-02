/**
 * @file logger.cpp
 * @brief Holds the definitions of non-template member functions of Logger class
 * and related utilities
 */

#include "logger.hpp"
#include <mutex>
#include <thread>

namespace ccerve {
namespace log {

std::vector<const Logger*>& Logger::getRegistry () {
    static std::vector<const Logger*> instances;
    return instances;
}

Logger::Logger (std::string_view p_LoggerName, LOG_LEVEL p_LogLevel, const SinksVector& p_Sinks)
: m_LoggerName (p_LoggerName), m_LogLevel (p_LogLevel), m_Sinks (p_Sinks),
  m_LogQueue (), m_StopWriteThread (false) {
    if (p_LoggerName == "")
        m_LoggerName = "Logger" + std::to_string (getRegistry ().size ());

    getRegistry ().push_back (this);
    m_WriteThread = std::thread ([this] { write (); });
}

Logger::~Logger () {
    auto registry = getRegistry ();
    registry.erase (
    std::remove (registry.begin (), registry.end (), this), registry.end ());

    m_StopWriteThread = true;
    m_CV.notify_one (); // since there will only ever me one write thread
    if (m_WriteThread.joinable ()) // ! I added this check just in case but is it
                                   // necessary? (When is a thread not joinable?)
        m_WriteThread.join ();
}

void Logger::write () const {
    using namespace std::chrono_literals;

    while (true) {
        std::unique_lock<std::mutex> queue_lock{ m_LogQueueMutex };

        /*
        The destructor of Logger class will set m_StopWriteThread to true
        and then call m_CV notify_one(). This will trigger the
        lambda function call and check if the condition is true. In this case,
        condition will be true (m_StopWriteThread = true). The while loop
        will empty the remaining elements and then finish the function
        */
        m_CV.wait (queue_lock, [this] { // ! is capturing "this" required?
            return m_StopWriteThread || !m_LogQueue.empty ();
        });

        // Process everything currently in the queue
        while (!m_LogQueue.empty ()) {
            std::string msg = std::move (m_LogQueue.front ());
            m_LogQueue.pop ();

            // This allows the main thread to keep pushing logs while we are
            // doing slow I/O.
            queue_lock.unlock ();

            for (auto& sink : m_Sinks) {
                sink->write (msg); // ! Multiple front() calls (one for each
                                   // sink). Is this fine?
            }
            // std::this_thread::sleep_for(1ms);

            queue_lock.lock (); // lock again (we are still in while loop)
        }

        // If we reach this point, we have ensured that m_LogQueue is empty
        if (m_StopWriteThread)
            break;

        // lock will be released when this scope ends
    }
}

void Logger::addSink (std::shared_ptr<sinks::BaseSink> p_Sink) {
    m_Sinks.push_back (p_Sink);
}

void Logger::setLogLevel (LOG_LEVEL p_LogLevel) {
    m_LogLevel = p_LogLevel;
}

LOG_LEVEL Logger::getLogLevel () {
    return m_LogLevel;
};

void Logger::setLoggerName (std::string_view p_LoggerName) {
    m_LoggerName = p_LoggerName;
};

std::string_view Logger::getLoggerName () {
    return m_LoggerName;
};

std::shared_ptr<Logger> getDefaultLogger () {
    static std::shared_ptr<Logger> DefaultLogger =
    std::make_shared<Logger> ("Default Logger");
    return DefaultLogger;
}

void setDefaultLogger (Logger* p_Logger) {
    getDefaultLogger ().reset (p_Logger);
}

} // namespace log
} // namespace ccerve
