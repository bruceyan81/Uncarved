#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        bool                       bBlocking,
        float                      rotationRadians,
        glm::ivec2                 position,
        glm::ivec2                 velocity,
        glm::fvec2                 scale,
        std::string                actorName,
        std::optional<glm::fvec2>  normalizedPivot,
        std::optional<std::string> viewTextureName
    )
        : bBlocking_(bBlocking)
        , rotationRadians_(rotationRadians)
        , position_(std::move(position))
        , velocity_(std::move(velocity))
        , scale_(std::move(scale))
        , actorName_(std::move(actorName))
        , normalizedPivot_(std::move(normalizedPivot))
        , viewTextureName_(std::move(viewTextureName))
    {
        id_ = actorCount_++;
    }

    Actor::Actor(Actor&& other) noexcept
        : bBlocking_(other.bBlocking_)
        , rotationRadians_(other.rotationRadians_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , position_(std::move(other.position_))
        , velocity_(std::move(other.velocity_))
        , scale_(std::move(other.scale_))
        , actorName_(std::move(other.actorName_))
        , normalizedPivot_(std::move(other.normalizedPivot_))
        , viewTextureName_(std::move(other.viewTextureName_))
    {
    }
} // namespace Uncarved::ObjectSpace
