#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        bool                      bBlocking,
        float                     rotationRadians,
        glm::ivec2                position,
        glm::ivec2                velocity,
        glm::fvec2                scale,
        std::string               actorName,
        std::optional<glm::fvec2> normalizedPivot,

        const ViewSpace::Sprite* outSprite
    )
        : bBlocking_(bBlocking)
        , rotationRadians_(rotationRadians)
        , position_(std::move(position))
        , velocity_(std::move(velocity))
        , scale_(std::move(scale))
        , actorName_(std::move(actorName))
        , normalizedPivotPoint_(std::move(normalizedPivot))
        , outSprite_(outSprite)
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
        , normalizedPivotPoint_(std::move(other.normalizedPivotPoint_))
        , outSprite_(other.outSprite_)
    {
    }
} // namespace Uncarved::ObjectSpace
