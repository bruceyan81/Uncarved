#pragma once

#include <optional>
#include <string>

namespace Uncarved::ObjectSpace
{
    struct DefinitionalActor final
    {
        int   x_{};
        int   y_{};
        int   zOrder_{0};
        float scaleX_{1.0f};
        float scaleY_{1.0f};
        float rotationRadians_{};

        std::string                actorName_{};
        std::optional<std::string> viewSpriteName_{};
    };
} // namespace Uncarved::ObjectSpace
