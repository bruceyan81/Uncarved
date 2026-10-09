#pragma once

namespace Uncarved::InputSpace
{
    enum class Key
    {
        None,
        W,
        A,
        S,
        D,
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,
        Enter,
        Space,
        Count
    };

    struct KeyState final
    {
        bool bDown_{false};
        bool bPressed_{false};
        bool bReleased_{false};
    };
} // namespace Uncarved::InputSpace
