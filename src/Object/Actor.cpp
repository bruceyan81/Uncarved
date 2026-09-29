#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        Transform2D transform2d,
        int         zOrder,
        std::string actorName,

        const ViewSpace::Sprite* outSprite
    )
        : transform2d_(transform2d)
        , zOrder_(std::move(zOrder))
        , actorName_(std::move(actorName))

        , outSprite_(outSprite)
    {
        id_ = actorCount_++;
    }

    Actor::Actor(Actor&& other) noexcept
        : transform2d_(other.transform2d_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , zOrder_(std::move(other.zOrder_))
        , actorName_(std::move(other.actorName_))

        , outSprite_(other.outSprite_)
    {
    }
} // namespace Uncarved::ObjectSpace
