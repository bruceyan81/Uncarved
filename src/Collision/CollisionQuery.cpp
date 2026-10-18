#include "CollisionQuery.h"

#include "CollisionGeometry.h"

namespace Uncarved::CollisionSpace
{
    std::optional<CollisionQuery> CollisionQuery::createCollisionQuery(
        const AxisAlignedBox&               bounds,
        const CollisionFilter&              filter,
        std::optional<ObjectSpace::ActorId> ignoredActorId,
        std::optional<CollisionShapeId>     ignoredShapeId
    )
    {
        if (!isValidAxisAlignedBox(bounds) || filter.category_ == CollisionCategory::None)
        {
            return std::nullopt;
        }

        return CollisionQuery{bounds, filter, ignoredActorId, ignoredShapeId};
    }
} // namespace Uncarved::CollisionSpace
