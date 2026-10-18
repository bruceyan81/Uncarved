#pragma once

#include <string_view>

namespace Uncarved
{
    enum class LogVerbosity
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical,
        Count
    };

    class LogCategory final
    {
    public:
        constexpr LogCategory(std::string_view name, LogVerbosity verbosityThreshold) noexcept
            : name_(name)
            , verbosityThreshold_(verbosityThreshold)
        {
        }

        constexpr std::string_view getName() const noexcept
        {
            return name_;
        }

        constexpr LogVerbosity getVerbosityThreshold() const noexcept
        {
            return verbosityThreshold_;
        }

        constexpr bool shouldLog(LogVerbosity verbosity) const noexcept
        {
            return verbosity != LogVerbosity::Count && verbosity >= verbosityThreshold_;
        }

    private:
        std::string_view name_;
        LogVerbosity     verbosityThreshold_;
    };

    extern const LogCategory gLogCore;
    extern const LogCategory gApplicationCore;
    extern const LogCategory gPlatform;
    extern const LogCategory gContent;
    extern const LogCategory gRuntime;
    extern const LogCategory gRender;
    extern const LogCategory gAudio;
} // namespace Uncarved
