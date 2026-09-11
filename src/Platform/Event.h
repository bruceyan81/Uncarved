#pragma once

#include <optional>

namespace Uncarved
{
    namespace PlatformSpace
    {
        class EventPump;
    }

    enum class EventType
    {
        None,
        Quit,
        WindowCloseRequested,
        KeyDown,
        MouseButtonDown,
        Count
    };

    enum class KeyboardScancode
    {
        Return,
        Space,
        Count
    };

    enum class MouseButton
    {
        Left,
        Middle,
        Right,
        Count
    };

    class Event final
    {
    public:
        EventType                       type_{EventType::None};
        std::optional<KeyboardScancode> keyScancode_{};
        bool                            keyRepeat_{false};
        std::optional<MouseButton>      mouseButton_{};
    };
} // namespace Uncarved
