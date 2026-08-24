#pragma once

#include <string>

namespace Uncarved::ObjectSpace
{
    struct DefinitionalActor final
    {
        bool        bBlocking_{false};
        std::string view_{"?"};
        int         x_{};
        int         y_{};
        int         velX_{};
        int         velY_{};
        std::string actorName_{};
        std::string nearbyDialogue_{};
        std::string contactDialogue_{};
    };
} // namespace Uncarved::ObjectSpace
