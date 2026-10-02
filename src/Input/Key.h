#pragma once

namespace Uncarved::InputSpace
{
    enum class Key
    {
        None,
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
}
