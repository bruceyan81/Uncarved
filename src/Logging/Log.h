#pragma once

#include "LogCategories.h"

#include <format>
#include <string>
#include <string_view>
#include <utility>

namespace Uncarved
{
    class Log final
    {
    public:
        static bool initialize() noexcept;

        static void shutdown() noexcept;

        static bool isInitialized() noexcept;

        template <typename... Args>
        static void write(
            const LogCategory&          logCategory,
            LogVerbosity                logVerbosity,
            std::format_string<Args...> format,
            Args&&... args
        ) noexcept
        {
            if (!logCategory.shouldLog(logVerbosity))
            {
                return;
            }

            try
            {
                std::string message = std::format(format, std::forward<Args>(args)...);

                writeLog(logCategory, logVerbosity, message);
            }
            catch (...)
            {
            }
        }

    private:
        static void
        writeLog(const LogCategory& logCategory, LogVerbosity logVerbosity, std::string_view logMessage) noexcept;
    };

} // namespace Uncarved

#define UC_LOG(Category, Verbosity, Format, ...)                                                                      \
    do                                                                                                                \
    {                                                                                                                 \
        const ::Uncarved::LogVerbosity ucInternalLogVerbosity = (Verbosity);                                  \
                                                                                                                      \
        const auto& ucInternalLogCategory = (Category);                                                               \
                                                                                                                      \
        if (ucInternalLogCategory.shouldLog(ucInternalLogVerbosity))                                                  \
        {                                                                                                             \
            ::Uncarved::Log::write(ucInternalLogCategory, ucInternalLogVerbosity, Format __VA_OPT__(, ) __VA_ARGS__); \
        }                                                                                                             \
    }                                                                                                                 \
    while (false)
