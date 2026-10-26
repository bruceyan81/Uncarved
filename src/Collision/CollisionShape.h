#pragma once

#include "CollisionTypes.h"

#include "Object/ObjectTypes.h"

#include <cstdint>
#include <limits>
#include <optional>

namespace Uncarved::GameSpace
{
    class World;
}

namespace Uncarved::ObjectSpace
{
    struct Transform2D;
}

namespace Uncarved::CollisionSpace
{
    class CollisionShape final
    {
    public:
        /**
         * @brief
         * Creates a Collision Shape owned by the specified Actor.
         *
         * @details
         * The Collision Shape's local geometry is defined in the owning Actor's Local Space.
         * Its initial World AABB is derived from the local geometry and the Actor's Transform2D.
         * Creating a Collision Shape does not register it with Collision World.
         *
         * @param ownerActorId
         * Identity of the owning Actor. The existence of the Actor is not verified.
         *
         * @param localCenter
         * Center of the collision geometry in Actor Local Space, expressed in WU.
         *
         * @param size
         * Width and height of the local collision geometry, expressed in WU.
         * Both components must be finite and strictly positive.
         *
         * @param actorTransform2d
         * Transform from Actor Local Space to World Space.
         * The current collision implementation supports only zero Actor rotation.
         *
         * @param filter
         * Collision filtering configuration.
         * The category must not be CollisionCategory::None.
         *
         * @param type
         * Collision geometry type.
         *
         * @param mode
         * Collision mode.
         *
         * @return
         * The created Collision Shape on success.
         * std::nullopt if the input is invalid, the transform is unsupported,
         * or the computed World AABB is invalid.
         */
        static std::optional<CollisionShape> createCollisionShape(
            ObjectSpace::ActorId            ownerActorId,
            const glm::fvec2&               localCenter,
            const glm::fvec2&               size,
            const ObjectSpace::Transform2D& actorTransform2d,
            const CollisionFilter&          filter,
            CollisionShapeType              type,
            CollisionMode                   mode
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

        std::optional<AxisAlignedBox>
        calculateCandidateWorldBound(const ObjectSpace::Transform2D& actorTransform2d) const noexcept;

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

        /**
         * @brief
         * Computes the World AABB from Actor-local collision geometry and the owning Actor's Transform2D.
         *
         * @details
         * This function performs no input or result validation
         * and does not modify committed collision state.
         *
         * @param localCenter
         * Center of the collision geometry in Actor Local Space, expressed in WU.
         *
         * @param size
         * Width and height of the local collision geometry, expressed in WU.
         *
         * @param actorTransform2d
         * Transform from Actor Local Space to World Space.
         *
         * @pre
         * The local center must have finite components.
         * Both size components must be finite and strictly positive.
         * The Actor's Transform2D must be valid and have zero rotation.
         *
         * @return
         * The computed World AABB, whose validity must be verified by the caller.
         */
        static AxisAlignedBox calculateWorldBound(
            const glm::fvec2&               localCenter,
            const glm::fvec2&               size,
            const ObjectSpace::Transform2D& actorTransform2d
        ) noexcept;

        void commitWorldBound(const AxisAlignedBox& worldBound) noexcept;

        friend class GameSpace::World;
    };
} // namespace Uncarved::CollisionSpace
