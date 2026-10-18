#include "CollisionShape.h"

#include "CollisionGeometry.h"

#include <cmath>
#include <utility>

namespace Uncarved::CollisionSpace
{
    std::optional<AxisAlignedBox> CollisionShape::calculateCandidateWorldBound(
        const glm::fvec2& actorPosition,
        const glm::fvec2& actorScale
    ) const noexcept
    {
        if (!CollisionSpace::isValidCollisionTransform(actorPosition, actorScale))
        {
            return std::nullopt;
        }

        const AxisAlignedBox worldBound =
            CollisionShape::calculateWorldBound(localCenter_, size_, actorPosition, actorScale);

        if (!isValidAxisAlignedBox(worldBound))
        {
            return std::nullopt;
        }

        return worldBound;
    }

    CollisionShape::CollisionShape(
        ObjectSpace::ActorId   ownerActorId,
        const glm::fvec2&      localCenter,
        const glm::fvec2&      size,
        const AxisAlignedBox&  worldBound,
        const CollisionFilter& filter,
        CollisionShapeType     type,
        CollisionMode          mode
    )
        : ownerActorId_(ownerActorId)
        , localCenter_(localCenter)
        , size_(size)
        , worldBound_(worldBound)
        , filter_(filter)
        , type_(type)
        , mode_(mode)
    {
        id_ = collisionShapeCount_++;
    }

    std::optional<CollisionShape> CollisionShape::createCollisionShape(
        ObjectSpace::ActorId   ownerActorId,
        const glm::fvec2&      localCenter,
        const glm::fvec2&      size,
        const glm::fvec2&      actorPosition,
        const glm::fvec2&      actorScale,
        const CollisionFilter& filter,
        CollisionShapeType     type,
        CollisionMode          mode
    )
    {

        if (!std::isfinite(localCenter.x) || !std::isfinite(localCenter.y) || !std::isfinite(size.x)
            || !std::isfinite(size.y) || size.x <= 0.0f || size.y <= 0.0f
            || !isValidCollisionTransform(actorPosition, actorScale) || filter.category_ == CollisionCategory::None)
        {
            return std::nullopt;
        }

        const auto worldBound = calculateWorldBound(localCenter, size, actorPosition, actorScale);

        if (!isValidAxisAlignedBox(worldBound))
        {
            return std::nullopt;
        }

        return CollisionShape(ownerActorId, localCenter, size, worldBound, filter, type, mode);
    }

    CollisionShape::CollisionShape(CollisionShape&& other) noexcept
        : ownerActorId_(std::exchange(other.ownerActorId_, kInvalidId_))
        , localCenter_(other.localCenter_)
        , size_(other.size_)
        , worldBound_(other.worldBound_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , filter_(other.filter_)
        , type_(other.type_)
        , mode_(other.mode_)
    {
    }

    CollisionShape& CollisionShape::operator=(CollisionShape&& other) noexcept
    {
        if (this != &other)
        {
            ownerActorId_ = std::exchange(other.ownerActorId_, kInvalidId_);
            localCenter_ = other.localCenter_;
            size_ = other.size_;
            worldBound_ = other.worldBound_;
            id_ = std::exchange(other.id_, kInvalidId_);
            filter_ = other.filter_;
            type_ = other.type_;
            mode_ = other.mode_;
        }

        return *this;
    }

    void CollisionSpace::CollisionShape::commitWorldBound(const AxisAlignedBox& worldBound) noexcept
    {
        worldBound_ = worldBound;
    }
} // namespace Uncarved::CollisionSpace
