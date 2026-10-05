#include "Log.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <filesystem>
#include <memory>
#include <system_error>
#include <utility>
#include <vector>

namespace Uncarved
{
    namespace
    {
        std::unique_ptr<spdlog::logger> gLogger;

        spdlog::level::level_enum convertVerbosity(LogVerbosity logVerbosity) noexcept
        {
            switch (logVerbosity)
            {
                case Uncarved::LogVerbosity::Trace:
                    return spdlog::level::trace;
                case Uncarved::LogVerbosity::Debug:
                    return spdlog::level::debug;
                case Uncarved::LogVerbosity::Info:
                    return spdlog::level::info;
                case Uncarved::LogVerbosity::Warning:
                    return spdlog::level::warn;
                case Uncarved::LogVerbosity::Error:
                    return spdlog::level::err;
                case Uncarved::LogVerbosity::Critical:
                    return spdlog::level::critical;
                default:
                    return spdlog::level::off;
            }
        }
    } // namespace

    bool Log::initialize() noexcept
    {
        if (gLogger != nullptr)
        {
            return true;
        }

        try
        {
            const std::filesystem::path logDirectory{"Saved/Logs"};
            const std::filesystem::path logFilePath = logDirectory / "Uncarved.log";

            std::error_code errorCode;
            std::filesystem::create_directories(logDirectory, errorCode);

            if (errorCode)
            {
                return false;
            }

            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);

            consoleSink->set_level(spdlog::level::trace);
            fileSink->set_level(spdlog::level::trace);

            // [HH:mm:ss.SSS] [Level]
            consoleSink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
            // [YYYY-MM-DD HH:mm:ss.SSS] [Level]
            fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

            std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

            auto logger = std::make_unique<spdlog::logger>("Uncarved", sinks.begin(), sinks.end());

            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::err);

            gLogger = std::move(logger);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    void Log::shutdown() noexcept
    {
        std::unique_ptr<spdlog::logger> logger = std::move(gLogger);

        if (logger == nullptr)
        {
            return;
        }

        try
        {
            logger->flush();
        }
        catch (...)
        {
        }
    }

    bool Log::isInitialized() noexcept
    {
        return gLogger != nullptr;
    }

    void Log::writeLog(const LogCategory& logCategory, LogVerbosity logVerbosity, std::string_view logMessage) noexcept
    {
        if (gLogger == nullptr)
        {
            return;
        }

        const auto logLevel = convertVerbosity(logVerbosity);

        if (logLevel == spdlog::level::off)
        {
            return;
        }

        try
        {
            gLogger->log(
                logLevel,
                "{}: {}",
                logCategory.getName(),
                logMessage
            );
        }
        catch (...)
        {
        }
    }
} // namespace Uncarved
