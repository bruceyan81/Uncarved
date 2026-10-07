#pragma once

#include "Content/GameContentLoader.h"
#include "Time/AppTime.h"

#include <memory>

namespace Uncarved::ApplicationSpace
{
    class Application;

    enum class ApplicationState
    {
        None,
        Init,
        Running,
        End,
        Count
    };

    class ApplicationCore final
    {
    public:
        ApplicationCore(std::unique_ptr<Application> application);

        ApplicationCore(const ApplicationCore&) = delete;
        ApplicationCore& operator=(const ApplicationCore&) = delete;

        ApplicationCore(ApplicationCore&&) = delete;
        ApplicationCore& operator=(ApplicationCore&&) = delete;

        ~ApplicationCore();

        int initializeApplication();

        int launch();

    private:
        ApplicationState                applicationState_{};
        ContentSpace::GameContentLoader gameContentLoader_{};
        TimeSpace::AppTime              appTime_{};
        std::unique_ptr<Application>    application_;
    };
} // namespace Uncarved::ApplicationSpace
