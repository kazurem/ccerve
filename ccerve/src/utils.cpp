/**
 * @file utils.cpp
 * @brief Implementation for general utilities
 */

#include "utils.hpp"

/*
 * Better way to do this than hardcoding the string's size? Have to make sure
 * that it works with the strftime function. I did some research and testing and
 * was unable to make it work with std::string.
 */

// @brief For holding the current time.
static time_t s_CurrentTime;

static const size_t s_CurrentTimeStrSize = 100;
// @brief String representation of the current time.
static char s_CurrentTimeStr[s_CurrentTimeStrSize];

static const size_t s_TimeFormatStrSize = 100;
// @brief Format in which time should be shown
static char s_TimeFormatStr[s_TimeFormatStrSize] = "%c";

const char* getCurrentTime () {
    time (&s_CurrentTime);
    std::strftime (s_CurrentTimeStr, s_CurrentTimeStrSize, s_TimeFormatStr,
    std::localtime (&s_CurrentTime));
    return s_CurrentTimeStr;
}
