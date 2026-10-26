#include "CollisionShape.h"

#include "CollisionGeometry.h"

#include "Object/Transform2D.h"

#include <cmath>
#include <utility>

namespace Uncarved::CollisionSpace
{
    std::optional<AxisAlignedBox>
    CollisionShape::calculateCandidateWorldBound(const ObjectSpace::Transform2D& actorTransform2d) const noexcept
    {
        if (!ObjectSpace::isValidTransform2D(actorTransform2d) || actorTransform2d.rotationRadians_ != 0.0f)
        {
            return std::nullopt;
        }

        const AxisAlignedBox worldBound = CollisionShape::calculateWorldBound(localCenter_, size_, actorTransform2d);

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
        ObjectSpace::ActorId            ownerActorId,
        const glm::fvec2&               localCenter,
        const glm::fvec2&               size,
        const ObjectSpace::Transform2D& actorTransform2d,
        const CollisionFilter&          filter,
        CollisionShapeType              type,
        CollisionMode                   mode
    )
    {

        if (!std::isfinite(localCenter.x) || !std::isfinite(localCenter.y)
            || !std::isfinite(size.x) || !std::isfinite(size.y)
            || size.x <= 0.0f || size.y <= 0.0f
            || !ObjectSpace::isValidTransform2D(actorTransform2d)
            || actorTransform2d.rotationRadians_ != 0.0f
            || filter.category_ == CollisionCategory::None)
        {
            return std::nullopt;
        }

        const auto worldBound = calculateWorldBound(localCenter, size, actorTransform2d);

        if (!isValidAxisAlignedBox(worldBound))
        {
            return std::nullopt;
        }

        return CollisionShape(ownerActorId, localCenter, size, worldBound, filter, type, mode);
    }

    AxisAlignedBox CollisionShape::calculateWorldBound(
        const glm::fvec2&               localCenter,
        const glm::fvec2&               size,
        const ObjectSpace::Transform2D& actorTransform2d
    ) noexcept
    {
        const glm::fvec2 worldCenter = actorTransform2d.position_ + localCenter * actorTransform2d.scale_;
        const glm::fvec2 scaledSize{
            size.x * std::abs(actorTransform2d.scale_.x),
            size.y * std::abs(actorTransform2d.scale_.y)
        };
        const glm::fvec2 min{worldCenter.x - scaledSize.x / 2, worldCenter.y - scaledSize.y / 2};
        const glm::fvec2 max{worldCenter.x + scaledSize.x / 2, worldCenter.y + scaledSize.y / 2};

        return AxisAlignedBox{min, max};
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
