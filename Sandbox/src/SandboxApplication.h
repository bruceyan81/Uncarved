#pragma once

#include "Application/Application.h"

namespace Sandbox
{
    class SandboxApplication final : public Uncarved::ApplicationSpace::Application
    {
    public:
        void onInitialize(Uncarved::ApplicationSpace::RuntimeContext&) override;
        void onUpdate(Uncarved::ApplicationSpace::RuntimeContext&) override;
        void onShutdown(Uncarved::ApplicationSpace::RuntimeContext&) override;
    };
}
