#pragma once

#include "CollisionTypes.h"

#include "Object/ObjectTypes.h"

#include <optional>

namespace Uncarved::CollisionSpace
{
    struct CollisionQuery final
    {
        static std::optional<CollisionQuery> createCollisionQuery(
            const AxisAlignedBox&                  bounds,
            const CollisionFilter&                 filter,
            std::optional<ObjectSpace::ActorId>    ignoredActorId = std::nullopt,
            std::optional<CollisionShapeId>         ignoredShapeId = std::nullopt
        );

        const AxisAlignedBox& getWorldBound() const noexcept
        {
            return worldBound_;
        }

        const CollisionFilter& getFilter() const noexcept
        {
            return filter_;
        }

        const std::optional<ObjectSpace::ActorId>& getIgnoredActorId() const noexcept
        {
            return ignoredActorId_;
        }

        const std::optional<CollisionShapeId>& getIgnoredShapeId() const noexcept
        {
            return ignoredShapeId_;
        }

    private:
        CollisionQuery(
            const AxisAlignedBox&                  bounds,
            const CollisionFilter&                 filter,
            std::optional<ObjectSpace::ActorId>    ignoredActorId,
            std::optional<CollisionShapeId>         ignoredShapeId
        )
            : worldBound_(bounds)
            , filter_(filter)
            , ignoredActorId_(ignoredActorId)
            , ignoredShapeId_(ignoredShapeId)
        {
        }

        AxisAlignedBox                      worldBound_{};
        CollisionFilter                     filter_{};
        std::optional<ObjectSpace::ActorId> ignoredActorId_{};
        std::optional<CollisionShapeId>     ignoredShapeId_{};
    };

    struct CollisionQueryResult final
    {
        AxisAlignedBox       worldBound_{};
        CollisionFilter      filter_{};
        ObjectSpace::ActorId ownerActorId_{};
        CollisionShapeId     shapeId_{};
        CollisionMode        mode_{};
        CollisionShapeType   shapeType_{};
    };
} // namespace Uncarved::CollisionSpace
