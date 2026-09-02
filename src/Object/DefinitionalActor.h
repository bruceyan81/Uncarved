#pragma once

#include <optional>
#include <string>

namespace Uncarved::ObjectSpace
{
    struct DefinitionalActor final
    {
        bool  bBlocking_{false};
        int   x_{};
        int   y_{};
        int   velX_{};
        int   velY_{};
        float scaleX_{1.0f};
        float scaleY_{1.0f};
        float rotationRadians_{};

        std::string                actorName_{};
        std::optional<std::string> viewTextureName_{};
        std::optional<float>       normalizedPivotX_;
        std::optional<float>       normalizedPivotY_;
    };
} // namespace Uncarved::ObjectSpace
