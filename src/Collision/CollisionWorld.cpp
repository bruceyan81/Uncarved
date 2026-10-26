#include "CollisionWorld.h"

#include "CollisionGeometry.h"
#include "CollisionProxy.h"
#include "CollisionQuery.h"
#include "CollisionShape.h"

namespace Uncarved::CollisionSpace
{
    bool CollisionWorld::registerShape(const CollisionShape& shape)
    {
        auto [it, inserted] = proxiesByShapeId_.try_emplace(
            shape.getCollisionShapeId(),
            CollisionProxy{
                shape.getWorldBound(),
                shape.getFilter(),
                shape.getOwnerActorId(),
                shape.getCollisionShapeId(),
                shape.getMode(),
                shape.getType()
            }
        );
        return inserted;
    }

    bool CollisionWorld::hasShape(CollisionShapeId shapeId) const noexcept
    {
        return proxiesByShapeId_.contains(shapeId);
    }

    bool CollisionWorld::updateShapeWorldBound(CollisionShapeId shapeId, const AxisAlignedBox& worldBound) noexcept
    {
        if (const auto it = proxiesByShapeId_.find(shapeId); it != proxiesByShapeId_.end())
        {
            it->second.commitWorldBound(worldBound);
            return true;
        }

        return false;
    }

    bool CollisionWorld::removeShape(CollisionShapeId shapeId)
    {
        return proxiesByShapeId_.erase(shapeId) == 1;
    }

    void CollisionWorld::reserveShapeCapacity(std::size_t shapeCapacity)
    {
        proxiesByShapeId_.reserve(shapeCapacity);
    }

    std::vector<CollisionQueryResult> CollisionWorld::queryOverlaps(const CollisionQuery& query) const
    {
        std::vector<CollisionQueryResult> queryResult{};

        for (auto& [shapeId, proxy] : proxiesByShapeId_)
        {
            if (query.getIgnoredActorId() == proxy.getOwnerActorId())
            {
                continue;
            }

            if (query.getIgnoredShapeId() == shapeId)
            {
                continue;
            }

            if (!canFiltersInteract(query.getFilter(), proxy.getFilter()))
            {
                continue;
            }

            if (!overlapsAxisAlignedBoxes(query.getWorldBound(), proxy.getWorldBound()))
            {
                continue;
            }

            queryResult.emplace_back(
                proxy.getWorldBound(),
                proxy.getFilter(),
                proxy.getOwnerActorId(),
                proxy.getShapeId(),
                proxy.getMode(),
                proxy.getShapeType()
            );
        }

        return queryResult;
    }
} // namespace Uncarved::CollisionSpace
