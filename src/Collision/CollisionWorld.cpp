#include "CollisionWorld.h"

#include "CollisionGeometry.h"
#include "CollisionProxy.h"
#include "CollisionQuery.h"
#include "CollisionShape.h"

namespace Uncarved::CollisionSpace
{
    // TODO Decide whether Uncarved supports C++ exceptions and define the project-wide exception handling policy
    bool CollisionWorld::registerShape(const CollisionShape& shape)
    {
        auto [proxyIt, bProxyInserted] = proxiesByShapeId_.try_emplace(
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

        if (!bProxyInserted)
        {
            return bProxyInserted;
        }

        if (!spatialHashGrid_.insert(proxyIt->first, proxyIt->second.worldBound_))
        {
            proxiesByShapeId_.erase(proxyIt);
            return false;
        }

        return true;
    }

    bool CollisionWorld::hasShape(CollisionShapeId shapeId) const noexcept
    {
        return proxiesByShapeId_.contains(shapeId) && spatialHashGrid_.hasShapeById(shapeId);
    }

    bool CollisionWorld::tryUpdateRegisteredShapeWorldBound(CollisionShapeId shapeId, const AxisAlignedBox& worldBound)
    {
        const auto proxyIt = proxiesByShapeId_.find(shapeId);

        if (proxyIt == proxiesByShapeId_.end())
        {
            return false;
        }

        if (!spatialHashGrid_.hasShapeById(shapeId))
        {
            return false;
        }

        if (!spatialHashGrid_.update(shapeId, worldBound))
        {
            return false;
        }

        proxyIt->second.commitWorldBound(worldBound);

        return true;
    }

    // TODO Add World-level runtime Actor and CollisionShape removal lifecycle support
    bool CollisionWorld::removeShape(CollisionShapeId shapeId)
    {
        if (!spatialHashGrid_.remove(shapeId))
        {
            return false;
        }

        return proxiesByShapeId_.erase(shapeId) == 1;
    }

    void CollisionWorld::reserveShapeCapacity(std::size_t shapeCapacity)
    {
        proxiesByShapeId_.reserve(shapeCapacity);
    }

    std::vector<CollisionQueryResult> CollisionWorld::queryOverlaps(const CollisionQuery& query) const
    {
        std::vector<CollisionQueryResult> queryResult{};
        const auto appendOverlapResult = [&query, &queryResult](CollisionShapeId shapeId, const CollisionProxy& proxy) {
            if (query.getIgnoredActorId() == proxy.getOwnerActorId())
            {
                return;
            }

            if (query.getIgnoredShapeId() == shapeId)
            {
                return;
            }

            if (!canFiltersInteract(query.getFilter(), proxy.getFilter()))
            {
                return;
            }

            if (!overlapsAxisAlignedBoxes(query.getWorldBound(), proxy.getWorldBound()))
            {
                return;
            }

            queryResult.emplace_back(
                proxy.getWorldBound(),
                proxy.getFilter(),
                proxy.getOwnerActorId(),
                proxy.getShapeId(),
                proxy.getMode(),
                proxy.getShapeType()
            );
        };

        const auto candidateShapeIds = spatialHashGrid_.queryCandidates(query.getWorldBound());

        if (candidateShapeIds.has_value())
        {
            for (const auto& shapeId : *candidateShapeIds)
            {
                auto proxyIt = proxiesByShapeId_.find(shapeId);

                if (proxyIt == proxiesByShapeId_.end())
                {
                    continue;
                }

                appendOverlapResult(shapeId, proxyIt->second);
            }
            return queryResult;
        }
        else
        {
            for (auto& [shapeId, proxy] : proxiesByShapeId_)
            {
                appendOverlapResult(shapeId, proxy);
            }
        }
        return queryResult;
    }
} // namespace Uncarved::CollisionSpace
