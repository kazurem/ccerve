using cpp_colors::foreground::green;
using cpp_colors::foreground::yellow;
using cpp_colors::foreground::red;
using cpp_colors::style::reset;

#define GREEN(p_Msg) std::format("{}{}{}", green, p_Msg, reset) 
#define YELLOW(p_Msg) std::format("{}{}{}", yellow, p_Msg, reset)
#define RED(p_Msg) std::format("{}{}{}", red, p_Msg, reset)

template<typename ...Args>
void Logger::log(std::string_view p_Msg, LOG_LEVEL p_LogLevel, std::string p_Tag, Args&& ...p_Args) const {
    if(p_LogLevel < m_LogLevel) return;

    // std::format requires compile time expressions but std::vformat does not
    auto user_formatted_str = std::vformat(p_Msg, std::make_format_args(p_Args...));

    {
        // push to log queue
        std::lock_guard<std::mutex> queue_lock{m_LogQueueMutex};
        m_LogQueue.push(std::format(m_LogFormat, getCurrentTime(), m_LoggerName, p_Tag, user_formatted_str));
    }

    m_CV.notify_one(); // wake up the write thread
};

template<typename ...Args>
void Logger::info(std::string_view p_Msg, Args&& ...p_Args) const {
    log(p_Msg, LOG_LEVEL::INFO, GREEN("info"), std::forward<Args>(p_Args)...);
};

template<typename ...Args>
void Logger::warn(std::string_view p_Msg, Args&& ...p_Args) const {
    log(p_Msg, LOG_LEVEL::WARN, YELLOW("warn"), std::forward<Args>(p_Args)...);
};

template<typename ...Args>
void Logger::error(std::string_view p_Msg, Args&& ...p_Args) const {
    log(p_Msg, LOG_LEVEL::ERROR, RED("error"), std::forward<Args>(p_Args)...);
};

template<typename ...Args>
void info(std::string_view p_Msg, Args&& ...p_Args) {
    getDefaultLogger()->info(p_Msg, std::forward<Args>(p_Args)...);
};

template<typename ...Args>
void warn(std::string_view p_Msg, Args&& ...p_Args) {
    getDefaultLogger()->warn(p_Msg, std::forward<Args>(p_Args)...);
};

template<typename ...Args>
void error(std::string_view p_Msg, Args&& ...p_Args) {
    getDefaultLogger()->error(p_Msg, std::forward<Args>(p_Args)...);
};
