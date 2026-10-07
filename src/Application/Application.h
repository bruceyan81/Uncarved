#pragma once

#include <memory>

namespace Uncarved::ApplicationSpace
{
    struct RuntimeContext;

    class Application
    {
    public:
        virtual ~Application() = default;

        virtual void onInitialize(RuntimeContext&) {};
        virtual void onUpdate(RuntimeContext&) = 0;
        virtual void onShutdown(RuntimeContext&) {};
    };

    std::unique_ptr<Application> createApplication();
}
