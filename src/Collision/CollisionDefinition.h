#pragma once

#include "CollisionTypes.h"

#include <glm/glm.hpp>

namespace Uncarved::CollisionSpace
{
    struct DefinitionalCollisionShape final
    {
        glm::fvec2      localCenter_{};
        glm::fvec2      size_{};
        CollisionMode   mode_{CollisionMode::Solid};
        CollisionFilter filter_{};
    };
} // namespace Uncarved::CollisionSpace
