#pragma once

#include "CollisionTypes.h"

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Uncarved::CollisionSpace
{
    class SpatialHashGrid final
    {
    public:
        bool insert(CollisionShapeId shapeId, const AxisAlignedBox& worldBound);
        bool update(CollisionShapeId shapeId, const AxisAlignedBox& newWorldBound);
        bool remove(CollisionShapeId shapeId);

        std::optional<std::vector<CollisionShapeId>> queryCandidates(const AxisAlignedBox& queryWorldBound) const;

    private:
        struct CellCoordinate final
        {
            std::int64_t x_{};
            std::int64_t y_{};

            bool operator==(const CellCoordinate&) const noexcept = default;
        };

        struct CellCoordinateHash final
        {
            std::size_t operator()(const CellCoordinate& coordinate) const noexcept;
        };

        using ShapeIds = std::vector<CollisionShapeId>;

        std::unordered_map<CellCoordinate, ShapeIds, CellCoordinateHash> shapeIdsByCell_;
        std::unordered_map<CollisionShapeId, AxisAlignedBox>             indexedWorldBoundsByShapeId_;
        std::unordered_set<CollisionShapeId>                             globalShapeIds_;
    };
} // namespace Uncarved::CollisionSpace
