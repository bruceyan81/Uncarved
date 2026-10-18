#pragma once

#include "CollisionTypes.h"

#include "Object/ObjectTypes.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>

namespace Uncarved::GameSpace
{
    class World;
}

namespace Uncarved::CollisionSpace
{
    class CollisionShape final
    {
    public:
        static std::optional<CollisionShape> createCollisionShape(
            ObjectSpace::ActorId   ownerActorId,
            const glm::fvec2&      localCenter,
            const glm::fvec2&      size,
            const glm::fvec2&      actorPosition,
            const glm::fvec2&      actorScale,
            const CollisionFilter& filter,
            CollisionShapeType     type,
            CollisionMode          mode
        );

        CollisionShape(const CollisionShape&) = delete;
        CollisionShape& operator=(const CollisionShape&) = delete;

        CollisionShape(CollisionShape&& other) noexcept;
        CollisionShape& operator=(CollisionShape&& other) noexcept;

        ObjectSpace::ActorId getOwnerActorId() const noexcept
        {
            return ownerActorId_;
        };

        const glm::fvec2 getLocalCenter() const noexcept
        {
            return localCenter_;
        }

        const glm::fvec2 getSize() const noexcept
        {
            return size_;
        }

        const AxisAlignedBox& getWorldBound() const noexcept
        {
            return worldBound_;
        };

        CollisionShapeId getCollisionShapeId() const noexcept
        {
            return id_;
        };

        const CollisionFilter& getFilter() const noexcept
        {
            return filter_;
        }

        CollisionShapeType getType() const noexcept
        {
            return type_;
        }

        CollisionMode getMode() const noexcept
        {
            return mode_;
        }

        std::optional<AxisAlignedBox> calculateCandidateWorldBound(
            const glm::fvec2& actorPosition,
            const glm::fvec2& actorScale
        ) const noexcept;

    private:
        inline static std::uint64_t    collisionShapeCount_ = 0;
        static constexpr std::uint64_t kInvalidId_ = std::numeric_limits<std::uint64_t>::max();

        ObjectSpace::ActorId ownerActorId_;

        glm::fvec2 localCenter_;
        glm::fvec2 size_;

        AxisAlignedBox     worldBound_;
        CollisionShapeId   id_;
        CollisionFilter    filter_;
        CollisionShapeType type_;
        CollisionMode      mode_;

        CollisionShape(
            ObjectSpace::ActorId   ownerActorId,
            const glm::fvec2&      localCenter,
            const glm::fvec2&      size,
            const AxisAlignedBox&  worldBound,
            const CollisionFilter& filter,
            CollisionShapeType     type,
            CollisionMode          mode
        );

        static AxisAlignedBox calculateWorldBound(
            const glm::fvec2& localCenter,
            const glm::fvec2& size,
            const glm::fvec2& actorPosition,
            const glm::fvec2& actorScale
        ) noexcept
        {
            glm::fvec2 worldCenter = actorPosition + localCenter * actorScale;
            glm::fvec2 scaledSize{size.x * std::abs(actorScale.x), size.y * std::abs(actorScale.y)};
            glm::fvec2 min{worldCenter.x - scaledSize.x / 2, worldCenter.y - scaledSize.y / 2};
            glm::fvec2 max{worldCenter.x + scaledSize.x / 2, worldCenter.y + scaledSize.y / 2};

            return AxisAlignedBox{min, max};
        }

        void commitWorldBound(const AxisAlignedBox& worldBound) noexcept;

        friend class GameSpace::World;
    };
} // namespace Uncarved::CollisionSpace
