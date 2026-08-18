#pragma once

#include "Content/GameContentLoader.h"

namespace Uncarved::ApplicationSpace
{
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
        ApplicationCore();

        ApplicationCore(const ApplicationCore&) = delete;
        ApplicationCore& operator=(const ApplicationCore&) = delete;

        ApplicationCore(ApplicationCore&&) = default;
        ApplicationCore& operator=(ApplicationCore&&) noexcept = default;

        ~ApplicationCore() = default;

        int initializeApplication();

        int launch();

    private:
        ApplicationState                applicationState_{};
        ContentSpace::GameContentLoader gameContentLoader_{};
    };
} // namespace Uncarved::ApplicationSpace
