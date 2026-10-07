#include "SandboxApplication.h"

#include "Application/RuntimeContext.h"

#include <memory>

namespace Uncarved::ApplicationSpace
{
    std::unique_ptr<Application> createApplication()
    {
        return std::make_unique<Sandbox::SandboxApplication>();
    }
} // namespace Uncarved::ApplicationSpace

namespace Sandbox
{
    void SandboxApplication::onInitialize(Uncarved::ApplicationSpace::RuntimeContext&)
    {
    }

    void SandboxApplication::onUpdate(Uncarved::ApplicationSpace::RuntimeContext&)
    {
    }

    void SandboxApplication::onShutdown(Uncarved::ApplicationSpace::RuntimeContext&)
    {
    }
}
