#pragma once

#include <glm/glm.hpp>

namespace Uncarved::ObjectSpace
{
    struct Transform2D final
    {
        glm::fvec2 position_{0.0f, 0.0f};
        glm::fvec2 scale_{1.0f, 1.0f};
        float      rotationRadians_{0};
    };
} // namespace Uncarved::ObjectSpace
