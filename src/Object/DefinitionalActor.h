#pragma once

#include "Object/Transform2D.h"

#include <optional>
#include <string>

namespace Uncarved::ObjectSpace
{
    struct DefinitionalActor final
    {
        ObjectSpace::Transform2D   transform2d_{};
        int                        zOrder_{0};
        std::string                actorName_{};
        std::optional<std::string> viewSpriteName_{};
    };
} // namespace Uncarved::ObjectSpace
