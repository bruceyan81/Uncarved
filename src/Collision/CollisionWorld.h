#pragma once

#include "CollisionProxy.h"
#include "CollisionQuery.h"
#include "CollisionTypes.h"
#include "SpatialHashGrid.h"

#include <cstddef>
#include <unordered_map>
#include <vector>

namespace Uncarved::GameSpace
{
    class World;
}

namespace Uncarved::CollisionSpace
{
    class CollisionShape;

    class CollisionWorld final
    {
    public:
        CollisionWorld() = default;

        CollisionWorld(const CollisionWorld&) = delete;
        CollisionWorld& operator=(const CollisionWorld&) = delete;

        CollisionWorld(CollisionWorld&&) noexcept = default;
        CollisionWorld& operator=(CollisionWorld&&) noexcept = default;

        std::vector<CollisionQueryResult> queryOverlaps(const CollisionQuery& query) const;

    private:
        std::unordered_map<CollisionShapeId, CollisionProxy> proxiesByShapeId_;
        SpatialHashGrid                                      spatialHashGrid_;

        bool registerShape(const CollisionShape& shape);
        bool hasShape(CollisionShapeId shapeId) const noexcept;
        bool updateShapeWorldBound(CollisionShapeId shapeId, const AxisAlignedBox& worldBound);
        bool removeShape(CollisionShapeId shapeId);
        void reserveShapeCapacity(std::size_t shapeCapacity);

        friend GameSpace::World;
    };
} // namespace Uncarved::CollisionSpace
