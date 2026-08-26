#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        bool        bBlocking,
        char        view,
        glm::ivec2  position,
        glm::ivec2  velocity,
        std::string actorName
    )
        : bBlocking_(bBlocking)
        , view_(view)
        , position_(std::move(position))
        , velocity_(std::move(velocity))
        , actorName_(std::move(actorName))
    {
        id_ = actorCount_++;
    }

    Actor::Actor(Actor&& other) noexcept
        : bBlocking_(other.bBlocking_)
        , view_(other.view_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , position_(std::move(other.position_))
        , velocity_(std::move(other.velocity_))
        , actorName_(std::move(other.actorName_))
    {
    }
} // namespace Uncarved::ObjectSpace
