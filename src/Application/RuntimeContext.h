#pragma once

namespace Uncarved::GameSpace
{
    class World;
}

namespace Uncarved::InputSpace
{
    class InputSystem;
}

namespace Uncarved::TimeSpace
{
    class AppTime;
}

namespace Uncarved::ApplicationSpace
{
    struct RuntimeContext final
    {
        GameSpace::World&              world_;
        const InputSpace::InputSystem& inputSystem_;
        const TimeSpace::AppTime&      appTime_;
    };
} // namespace Uncarved::ApplicationSpace
