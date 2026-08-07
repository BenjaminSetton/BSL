#pragma once

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // fprintf_s, vsprintf_s
#include <cstring> // memcpy
#include <ctime>   // std::time, std::tm, localtime_s
#include <chrono>

#include "integral_types.h"
#include "memory.h"
#include "sanity.h"

// Internal helper macro
#define BSL_LOG_FORMAT_CODE(logType) \
    va_list va; \
    va_start(va, format); \
    vsprintf_s(s_printBuffer, MAX_BUFFER_SIZE, format, va); \
    Log_Internal(logType, s_printBuffer); \
    va_end(va);

namespace BSL
{
    enum class LOG_TYPE { DBG, INFO, WARNING, ERR };

    inline constexpr u64 MAX_BUFFER_SIZE = KB(16);
    inline char s_printBuffer[MAX_BUFFER_SIZE];

    // ANSI color codes
    inline const char* const LogErrorColor   = "\x1B[31m";
    inline const char* const LogWarningColor = "\x1B[33m";
    inline const char* const LogInfoColor    = "\x1B[37m";
    inline const char* const LogDebugColor   = "\x1B[36m";
    inline const char* const LogClearColor   = "\033[0m";

    // Log type buffers
    inline const char* const LogErrorBuffer   = "ERROR";
    inline const char* const LogWarningBuffer = "WARNING";
    inline const char* const LogInfoBuffer    = "INFO";
    inline const char* const LogDebugBuffer   = "DEBUG";

    inline void Log_Internal(LOG_TYPE type, const char* message)
    {
        const char* logTypeColor = nullptr;
        const char* logTypeBuffer = nullptr;

        switch (type)
        {
        case LOG_TYPE::DBG:
        {
            logTypeColor = LogDebugColor;
            logTypeBuffer = LogDebugBuffer;
            break;
        }
        case LOG_TYPE::INFO:
        {
            logTypeColor = LogInfoColor;
            logTypeBuffer = LogInfoBuffer;
            break;
        }
        case LOG_TYPE::WARNING:
        {
            logTypeColor = LogWarningColor;
            logTypeBuffer = LogWarningBuffer;
            break;
        }
        case LOG_TYPE::ERR:
        {
            logTypeColor = LogErrorColor;
            logTypeBuffer = LogErrorBuffer;
            break;
        }
        }

        // Get the timestamp with millisecond precision
		auto now = std::chrono::system_clock::now();
		auto milliseconds = static_cast<u32>((std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count());

        TECHDEBT("This is windows-only. Use localtime_r for POSIX");
		std::time_t t = std::chrono::system_clock::to_time_t(now);
		std::tm currentLocalTime{};
		localtime_s(&currentLocalTime, &t);

        int hour = currentLocalTime.tm_hour;
        char AMorPM[3] = "AM"; // Contains "AM" or "PM" including null terminator

        if (hour == 0)
        {
            // Edge-case for midnight
            hour = 12;
        }
        if (hour > 12)
        {
            hour -= 12;
            memcpy(AMorPM, "PM", 3);
        }

        fprintf_s(stderr, "[%02d:%02d:%02d:%03d %s]%s[%s] %s %s\n",
            hour,
            currentLocalTime.tm_min,
            currentLocalTime.tm_sec,
            milliseconds,
            AMorPM,
            logTypeColor,
            logTypeBuffer,
            message,
            LogClearColor);
    }

    inline void LogError(const char* format, ...)
    {
        BSL_LOG_FORMAT_CODE(LOG_TYPE::ERR);
    }

    inline void LogWarning(const char* format, ...)
    {
        BSL_LOG_FORMAT_CODE(LOG_TYPE::WARNING);
    }

    inline void LogInfo(const char* format, ...)
    {
        BSL_LOG_FORMAT_CODE(LOG_TYPE::INFO);
    }

    inline void LogDebug(const char* format, ...)
    {
        BSL_LOG_FORMAT_CODE(LOG_TYPE::DBG);
    }
}
