#include "CollisionProxy.h"

namespace Uncarved::CollisionSpace
{
    CollisionProxy::CollisionProxy(
        const AxisAlignedBox&  worldBound,
        const CollisionFilter& filter,
        ObjectSpace::ActorId   ownerActorId,
        CollisionShapeId       shapeId,
        CollisionMode          mode,
        CollisionShapeType     shapeType
    )
        : worldBound_(worldBound)
        , filter_(filter)
        , ownerActorId_(ownerActorId)
        , shapeId_(shapeId)
        , mode_(mode)
        , shapeType_(shapeType)
    {
    }

    void CollisionProxy::commitWorldBound(const AxisAlignedBox& worldBound) noexcept
    {
        worldBound_ = worldBound;
    }
} // namespace Uncarved::CollisionSpace
