#include "SpatialHashGrid.h"

#include "CollisionGeometry.h"
#include "CollisionTypes.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <unordered_set>

namespace Uncarved::CollisionSpace
{
    namespace
    {
        /**
         * @brief Describes whether a World AABB can use normal Spatial Hash Grid cell enumeration
         */
        enum class CellRangeStatus
        {
            Normal,
            // Unrepresentable identifies valid bounds that cannot be safely converted to Cell Coordinates
            Unrepresentable,
            // Oversized identifies valid and representable bounds whose cell coverage exceeds the enumeration limit
            Oversized,
            // Invalid identifies an invalid World AABB
            Invalid
        };

        struct OneDimensionalRange final
        {
            std::int64_t minCellIndex_{};
            std::int64_t maxCellIndex_{};
        };

        struct TwoDimensionalRange final
        {
            OneDimensionalRange horizontal_{};
            OneDimensionalRange vertical_{};
        };

        bool isContainedCell(const TwoDimensionalRange& range, std::int64_t x, std::int64_t y)
        {
            return x >= range.horizontal_.minCellIndex_ && x <= range.horizontal_.maxCellIndex_
                && y >= range.vertical_.minCellIndex_ && y <= range.vertical_.maxCellIndex_;
        }

        struct CellRangeResult final
        {
            TwoDimensionalRange range_{};
            CellRangeStatus     status_{CellRangeStatus::Invalid};
            std::size_t         cellCount_{};
        };

        constexpr double kCellSize = 10.0f;
        constexpr double kMinCellIndexInclusive = static_cast<double>(std::numeric_limits<std::int64_t>::min());
        constexpr double kMaxCellIndexExclusive = -kMinCellIndexInclusive;
        constexpr std::size_t kMaxCellCount = 1024;

        CellRangeResult toTwoDimensionalRange(const AxisAlignedBox& box)
        {
            if (!isValidAxisAlignedBox(box))
            {
                return CellRangeResult{TwoDimensionalRange{}, CellRangeStatus::Invalid};
            }

            const glm::fvec2 min = box.min_;
            const glm::fvec2 max = box.max_;

            const double horizontalMinCellIndex = std::floor(min.x / kCellSize);
            const double horizontalMaxCellIndex = std::ceil(max.x / kCellSize) - 1.0;
            const double verticalMinCellIndex = std::floor(min.y / kCellSize);
            const double verticalMaxCellIndex = std::ceil(max.y / kCellSize) - 1.0;

            if (horizontalMinCellIndex >= kMaxCellIndexExclusive || horizontalMinCellIndex < kMinCellIndexInclusive
                || horizontalMaxCellIndex < kMinCellIndexInclusive || horizontalMaxCellIndex >= kMaxCellIndexExclusive
                || verticalMinCellIndex >= kMaxCellIndexExclusive || verticalMinCellIndex < kMinCellIndexInclusive
                || verticalMaxCellIndex < kMinCellIndexInclusive || verticalMaxCellIndex >= kMaxCellIndexExclusive)
            {
                return CellRangeResult{TwoDimensionalRange{}, CellRangeStatus::Unrepresentable};
            }

            const double horizontalCellCount = (horizontalMaxCellIndex - horizontalMinCellIndex) + 1.0;
            const double verticalCellCount = (verticalMaxCellIndex - verticalMinCellIndex) + 1.0;

            const double cellCount = horizontalCellCount * verticalCellCount;
            if (horizontalCellCount > kMaxCellCount || verticalCellCount > kMaxCellCount || cellCount > kMaxCellCount)
            {
                return CellRangeResult{TwoDimensionalRange{}, CellRangeStatus::Oversized};
            }

            return CellRangeResult{
                TwoDimensionalRange{
                    OneDimensionalRange{
                        static_cast<std::int64_t>(horizontalMinCellIndex),
                        static_cast<std::int64_t>(horizontalMaxCellIndex)
                    },
                    OneDimensionalRange{
                        static_cast<std::int64_t>(verticalMinCellIndex),
                        static_cast<std::int64_t>(verticalMaxCellIndex)
                    }
                },
                CellRangeStatus::Normal,
                static_cast<std::size_t>(cellCount)
            };
        }

        template <typename Visitor>
        bool visitCells(const TwoDimensionalRange& range, Visitor&& visitor)
        {
            const auto&  horizontalRange = range.horizontal_;
            const auto&  verticalRange = range.vertical_;

            std::int64_t y = verticalRange.minCellIndex_;

            while (true)
            {
                std::int64_t x = horizontalRange.minCellIndex_;

                while (true)
                {
                    if (!visitor(x, y))
                    {
                        return false;
                    }

                    if (x == horizontalRange.maxCellIndex_)
                    {
                        break;
                    }
                    ++x;
                }

                if (y == verticalRange.maxCellIndex_)
                {
                    break;
                }
                ++y;
            }

            return true;
        }
    } // namespace

    bool SpatialHashGrid::insert(CollisionShapeId shapeId, const AxisAlignedBox& worldBound)
    {
        if (indexedWorldBoundsByShapeId_.contains(shapeId))
        {
            return false;
        }

        CellRangeResult result = toTwoDimensionalRange(worldBound);

        switch (result.status_)
        {
            case CellRangeStatus::Normal:
            {
                const auto rollbackCellMemberships = [shapeId, &result, this]() {
                    visitCells(
                        result.range_,
                        [shapeId, this](std::int64_t x, std::int64_t y)
                        {
                            const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{x, y});

                            if (cellIt == this->shapeIdsByCell_.end())
                            {
                                return true;
                            }

                            std::erase(cellIt->second, shapeId);

                            if (cellIt->second.empty())
                            {
                                this->shapeIdsByCell_.erase(cellIt);
                            }

                            return true;
                        }
                    );
                };

                try
                {
                    const bool bVisitedAllCells = visitCells(
                        result.range_,
                        [shapeId, this](std::int64_t x, std::int64_t y)
                        {
                            this->shapeIdsByCell_[CellCoordinate{x, y}].push_back(shapeId);
                            return true;
                        }
                    );

                    if (!bVisitedAllCells)
                    {
                        rollbackCellMemberships();
                        return false;
                    }

                    const bool bInserted = indexedWorldBoundsByShapeId_.emplace(shapeId, worldBound).second;

                    if (!bInserted)
                    {
                        rollbackCellMemberships();
                        return false;
                    }

                    return true;
                }
                catch (...)
                {
                    rollbackCellMemberships();
                    throw;
                }
            }

            case CellRangeStatus::Unrepresentable:
            case CellRangeStatus::Oversized:
            {
                if (indexedWorldBoundsByShapeId_.contains(shapeId))
                {
                    return false;
                }

                bool bGlobalInserted = false;

                try
                {
                    bGlobalInserted = globalShapeIds_.insert(shapeId).second;
                    if (!bGlobalInserted)
                    {
                        return false;
                    }

                    const bool bRecordInserted = indexedWorldBoundsByShapeId_.emplace(shapeId, worldBound).second;

                    if (!bRecordInserted)
                    {
                        globalShapeIds_.erase(shapeId);
                        return false;
                    }

                    return true;
                }
                catch (...)
                {
                    if (bGlobalInserted)
                    {
                        globalShapeIds_.erase(shapeId);
                    }
                    throw;
                }
            }

            case CellRangeStatus::Invalid:
                return false;

            default:
                return false;
        }
    }

    bool SpatialHashGrid::update(CollisionShapeId shapeId, const AxisAlignedBox& newWorldBound)
    {
        const auto recordIt = indexedWorldBoundsByShapeId_.find(shapeId);

        if (recordIt == indexedWorldBoundsByShapeId_.end())
        {
            return false;
        }

        const AxisAlignedBox oldWorldBound = recordIt->second;

        const CellRangeResult oldResult = toTwoDimensionalRange(oldWorldBound);
        const CellRangeResult newResult = toTwoDimensionalRange(newWorldBound);

        if (newResult.status_ == CellRangeStatus::Invalid)
        {
            return false;
        }

        // Both normal
        if (newResult.status_ == CellRangeStatus::Normal && newResult.status_ == oldResult.status_)
        {
            // Verify that every Cell scheduled for removal contains exactly one matching ShapeId
            const bool bHasCompleteOldMembership =
                visitCells(
                    oldResult.range_,
                    [&newResult, shapeId, this](std::int64_t oldX, std::int64_t oldY)
                    {
                        if (isContainedCell(newResult.range_, oldX, oldY))
                        {
                            return true;
                        }

                        const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{oldX, oldY});

                        if (cellIt == this->shapeIdsByCell_.end())
                        {
                            return false;
                        }

                        return std::count(cellIt->second.begin(), cellIt->second.end(), shapeId) == 1;
                    }
                );

            if (!bHasCompleteOldMembership)
            {
                return false;
            }

            // Verify that every Cell scheduled for insertion does not contain the ShapeId
            const bool bHasNoNewMembership =
                visitCells(
                    newResult.range_,
                    [this, &oldResult, shapeId](std::int64_t newX, std::int64_t newY)
                    {
                        if (isContainedCell(oldResult.range_, newX, newY))
                        {
                            return true;
                        }

                        const auto cellIt = shapeIdsByCell_.find(CellCoordinate{newX, newY});

                        if (cellIt == shapeIdsByCell_.end())
                        {
                            return true;
                        }

                        return std::count(cellIt->second.begin(), cellIt->second.end(), shapeId) == 0;
                });

            if (!bHasNoNewMembership)
            {
                return false;
            }

            const auto rollbackNewMemberships = [&oldResult, &newResult, shapeId, this]() {
                visitCells(
                    newResult.range_,
                    [&oldResult, shapeId, this](std::int64_t newX, std::int64_t newY)
                    {
                        if (isContainedCell(oldResult.range_, newX, newY))
                        {
                            return true;
                        }

                        const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{newX, newY});

                        if (cellIt == this->shapeIdsByCell_.end())
                        {
                            return true;
                        }

                        std::erase(cellIt->second, shapeId);

                        if (cellIt->second.empty())
                        {
                            this->shapeIdsByCell_.erase(cellIt);
                        }

                        return true;
                    }
                );
            };

            // Add new shape id
            {
                try
                {
                    const bool bVisitedAllCells = visitCells(
                        newResult.range_,
                        [&oldResult, &shapeId, this](std::int64_t newX, std::int64_t newY)
                        {
                            if (!isContainedCell(oldResult.range_, newX, newY))
                            {
                                this->shapeIdsByCell_[CellCoordinate{newX, newY}].push_back(shapeId);
                            }

                            return true;
                        }
                    );

                    if (!bVisitedAllCells)
                    {
                        rollbackNewMemberships();
                        return false;
                    }
                }
                catch (...)
                {
                    rollbackNewMemberships();
                    throw;
                }
            }

            // Remove old shape id
            {
                // Commit removal of the previously validated old Cell Memberships
                visitCells(
                    oldResult.range_,
                    [&newResult, shapeId, this](std::int64_t oldX, std::int64_t oldY)
                    {
                        if (isContainedCell(newResult.range_, oldX, oldY))
                        {
                            return true;
                        }

                        const auto cellIt = shapeIdsByCell_.find(CellCoordinate{oldX, oldY});

                        std::erase(cellIt->second, shapeId);

                        if (cellIt->second.empty())
                        {
                            shapeIdsByCell_.erase(cellIt);
                        }

                        return true;
                    }
                );
            }
        }
        // Before normal, after global
        else if (
            (newResult.status_ == CellRangeStatus::Oversized || newResult.status_ == CellRangeStatus::Unrepresentable)
            && oldResult.status_ == CellRangeStatus::Normal
        )
        {
            if (globalShapeIds_.contains(shapeId))
            {
                return false;
            }

            // Verify that every old Cell contains exactly one matching ShapeId
            const bool bHasCompleteOldMembership = visitCells(
                oldResult.range_,
                [shapeId, this](std::int64_t oldX, std::int64_t oldY)
                {
                    const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{oldX, oldY});

                    if (cellIt == this->shapeIdsByCell_.end())
                    {
                        return false;
                    }

                    return std::count(cellIt->second.begin(), cellIt->second.end(), shapeId) == 1;
                }
            );

            if (!bHasCompleteOldMembership)
            {
                return false;
            }

            const bool bGlobalInserted = globalShapeIds_.insert(shapeId).second;

            if (!bGlobalInserted)
            {
                return false;
            }

            // Commit removal of the previously validated old Cell Memberships
            visitCells(
                oldResult.range_,
                [shapeId, this](std::int64_t oldX, std::int64_t oldY)
                {
                    const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{oldX, oldY});

                    std::erase(cellIt->second, shapeId);

                    if (cellIt->second.empty())
                    {
                        this->shapeIdsByCell_.erase(cellIt);
                    }

                    return true;
                }
            );
        }
        // Before global, after normal
        else if (
            newResult.status_ == CellRangeStatus::Normal
            && (oldResult.status_ == CellRangeStatus::Oversized || oldResult.status_ == CellRangeStatus::Unrepresentable)
        )
        {
            if (!globalShapeIds_.contains(shapeId))
            {
                return false;
            }

            // Verify that every Cell scheduled for insertion does not contain the ShapeId
            const bool bHasNoNewMembership = visitCells(
                newResult.range_,
                [shapeId, this](std::int64_t newX, std::int64_t newY)
                {
                    const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{newX, newY});

                    if (cellIt == this->shapeIdsByCell_.end())
                    {
                        return true;
                    }

                    return std::count(cellIt->second.begin(), cellIt->second.end(), shapeId) == 0;
                }
            );

            if (!bHasNoNewMembership)
            {
                return false;
            }

            const auto rollbackNewMemberships = [&newResult, shapeId, this]() {
                visitCells(
                    newResult.range_,
                    [shapeId, this](std::int64_t newX, std::int64_t newY)
                    {
                        const auto cellIt = this->shapeIdsByCell_.find(CellCoordinate{newX, newY});

                        if (cellIt == this->shapeIdsByCell_.end())
                        {
                            return true;
                        }

                        std::erase(cellIt->second, shapeId);

                        if (cellIt->second.empty())
                        {
                            this->shapeIdsByCell_.erase(cellIt);
                        }

                        return true;
                    }
                );
            };

            try
            {
                const bool bVisitedAllCells = visitCells(
                    newResult.range_,
                    [shapeId, this](std::int64_t newX, std::int64_t newY)
                    {
                        this->shapeIdsByCell_[CellCoordinate{newX, newY}].push_back(shapeId);
                        return true;
                    }
                );

                if (!bVisitedAllCells)
                {
                    rollbackNewMemberships();
                    return false;
                }
            }
            catch (...)
            {
                rollbackNewMemberships();
                throw;
            }

            globalShapeIds_.erase(shapeId);
        }
        // Both global
        else if (
            (newResult.status_ == CellRangeStatus::Oversized || newResult.status_ == CellRangeStatus::Unrepresentable)
            && (oldResult.status_ == CellRangeStatus::Oversized || oldResult.status_ == CellRangeStatus::Unrepresentable)
        )
        {
            if (!globalShapeIds_.contains(shapeId))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        recordIt->second = newWorldBound;

        return true;
    }

    bool SpatialHashGrid::remove(CollisionShapeId shapeId)
    {
        const auto recordIt = indexedWorldBoundsByShapeId_.find(shapeId);

        if (recordIt == indexedWorldBoundsByShapeId_.end())
        {
            return false;
        }

        const CellRangeResult result = toTwoDimensionalRange(recordIt->second);

        if (result.status_ == CellRangeStatus::Invalid)
        {
            return false;
        }

        if (result.status_ == CellRangeStatus::Normal)
        {
            const bool bHasCompleteMembership = visitCells(
                result.range_,
                [&shapeId, this](std::int64_t x, std::int64_t y)
                {
                    const auto cellIt = shapeIdsByCell_.find(CellCoordinate{x, y});

                    if (cellIt == shapeIdsByCell_.end())
                    {
                        return false;
                    }

                    return std::count(cellIt->second.begin(), cellIt->second.end(), shapeId) == 1;
                }
            );

            if (!bHasCompleteMembership)
            {
                return false;
            }

            visitCells(
                result.range_,
                [&shapeId, this](std::int64_t x, std::int64_t y)
                {
                    const auto cellIt = shapeIdsByCell_.find(CellCoordinate{x, y});

                    std::erase(cellIt->second, shapeId);

                    if (cellIt->second.empty())
                    {
                        shapeIdsByCell_.erase(cellIt);
                    }

                    return true;
                }
            );
        }
        // Global
        else
        {
            if (!globalShapeIds_.contains(shapeId))
            {
                return false;
            }

            const std::size_t erasedCount = globalShapeIds_.erase(shapeId);

            if (erasedCount != 1)
            {
                return false;
            }
        }

        indexedWorldBoundsByShapeId_.erase(recordIt);
        return true;
    }

    std::optional<std::vector<CollisionShapeId>>
    SpatialHashGrid::queryCandidates(const AxisAlignedBox& queryWorldBound) const
    {
        CellRangeResult result = toTwoDimensionalRange(queryWorldBound);
        switch (result.status_)
        {
            case CellRangeStatus::Normal:
            {
                std::unordered_set<CollisionShapeId> candidates{};
                candidates.insert(globalShapeIds_.begin(), globalShapeIds_.end());

                visitCells(
                    result.range_,
                    [&candidates, this](std::int64_t x, std::int64_t y)
                    {
                        CellCoordinate coordinate{x, y};

                        const auto it = shapeIdsByCell_.find(coordinate);

                        if (it != shapeIdsByCell_.end())
                        {
                            candidates.insert(it->second.begin(), it->second.end());
                        }

                        return true;
                    }
                );

                return std::vector<CollisionShapeId>{candidates.begin(), candidates.end()};
            }

            case CellRangeStatus::Unrepresentable:
            case CellRangeStatus::Oversized:
            {
                std::vector<CollisionShapeId> candidates{};
                candidates.reserve(indexedWorldBoundsByShapeId_.size());

                for (const auto& [shapeId, worldBound] : indexedWorldBoundsByShapeId_)
                {
                    candidates.push_back(shapeId);
                    (void)worldBound;
                }

                return candidates;
            }

            case CellRangeStatus::Invalid:
                return std::nullopt;

            default:
                return std::nullopt;
        }
    }

    std::size_t SpatialHashGrid::CellCoordinateHash::operator()(const CellCoordinate& coordinate) const noexcept
    {
        const std::size_t horizontalHash = std::hash<std::int64_t>{}(coordinate.x_);
        const std::size_t verticalHash = std::hash<std::int64_t>{}(coordinate.y_);

        return horizontalHash ^ (verticalHash + 0x9e3779b9U + (horizontalHash << 6) + (horizontalHash >> 2));
    }
} // namespace Uncarved::CollisionSpace
