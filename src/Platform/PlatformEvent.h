#pragma once

#include <optional>

namespace Uncarved
{
    enum class PlatformEventType
    {
        None,
        Quit,
        WindowCloseRequested,
        KeyPressed,
        KeyReleased,
        KeyRepeat,
        MouseButtonDown,
        Count
    };

    /**
     * @brief Represents a physical keyboard key position recognized by the Platform layer.
     *
     * This enum converts keyboard input representations provided
     * by a specific platform or input backend into a unified and stable keyboard scancode representation used
     * by the Uncarved Platform layer.
     *
     * It identifies which physical keyboard key was operated
     * and does not represent characters, text input, or Gameplay semantics.
     */
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

    class PlatformEvent final
    {
    public:
        PlatformEventType               type_{PlatformEventType::None};
        std::optional<KeyboardScancode> keyScancode_{};
        std::optional<MouseButton>      mouseButton_{};
    };
} // namespace Uncarved
