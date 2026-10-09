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

namespace Uncarved::ViewSpace
{
    class Camera2D;
}

namespace Uncarved::ApplicationSpace
{
    struct RuntimeContext final
    {
        GameSpace::World&              outWorld_;
        ViewSpace::Camera2D&           outCamera2d_;
        const InputSpace::InputSystem& outInputSystem_;
        const TimeSpace::AppTime&      outAppTime_;
    };
} // namespace Uncarved::ApplicationSpace
