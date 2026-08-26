#pragma once

#include "Object/Actor.h"

#include <glm/glm.hpp>

#include <variant>

namespace Uncarved::GameSpace
{
    struct MoveActorCommand final
    {
        ObjectSpace::ActorId actorId_;
        glm::ivec2           delta_;
    };

    struct SetActorVelocityCommand final
    {
        ObjectSpace::ActorId actorId_;
        glm::ivec2           velocity_;
    };

    struct QuitCommand final
    {
        // placeholder
    };

    using RuntimeCommand = std::variant<MoveActorCommand, SetActorVelocityCommand, QuitCommand>;
} // namespace Uncarved::GameSpace
