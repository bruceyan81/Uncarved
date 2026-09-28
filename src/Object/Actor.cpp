#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        float       rotationRadians,
        glm::ivec2  position,
        int         zOrder,
        glm::fvec2  scale,
        std::string actorName,

        const ViewSpace::Sprite* outSprite
    )
        : rotationRadians_(rotationRadians)
        , position_(std::move(position))
        , zOrder_(std::move(zOrder))
        , scale_(std::move(scale))
        , actorName_(std::move(actorName))

        , outSprite_(outSprite)
    {
        id_ = actorCount_++;
    }

    Actor::Actor(Actor&& other) noexcept
        : rotationRadians_(other.rotationRadians_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , position_(std::move(other.position_))
        , zOrder_(std::move(other.zOrder_))
        , scale_(std::move(other.scale_))
        , actorName_(std::move(other.actorName_))

        , outSprite_(other.outSprite_)
    {
    }
} // namespace Uncarved::ObjectSpace
