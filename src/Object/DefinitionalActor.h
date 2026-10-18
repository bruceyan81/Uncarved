#pragma once

#include "Collision/CollisionDefinition.h"
#include "Object/Transform2D.h"

#include <optional>
#include <string>
#include <vector>

namespace Uncarved::ObjectSpace
{
    struct DefinitionalActor final
    {
        ObjectSpace::Transform2D   transform2d_{};
        int                        zOrder_{0};
        std::string                actorName_{};
        std::optional<std::string> viewSpriteName_{};
        std::vector<CollisionSpace::DefinitionalCollisionShape> collisionShapes_{};
    };
} // namespace Uncarved::ObjectSpace
