#pragma once

#include "CollisionTypes.h"

#include "Object/ObjectTypes.h"

namespace Uncarved::CollisionSpace
{
    class CollisionWorld;

    class CollisionProxy final
    {
    public:
        CollisionProxy(
            const AxisAlignedBox&  worldBound,
            const CollisionFilter& filter,
            ObjectSpace::ActorId   ownerActorId,
            CollisionShapeId       shapeId,
            CollisionMode          mode,
            CollisionShapeType     shapeType
        );

        CollisionProxy(const CollisionProxy&) = delete;
        CollisionProxy& operator=(const CollisionProxy&) = delete;

        CollisionProxy(CollisionProxy&& other) noexcept = default;
        CollisionProxy& operator=(CollisionProxy&& other) noexcept = default;

        const AxisAlignedBox& getWorldBound() const noexcept
        {
            return worldBound_;
        };

        const CollisionFilter& getFilter() const noexcept
        {
            return filter_;
        };

        ObjectSpace::ActorId getOwnerActorId() const noexcept
        {
            return ownerActorId_;
        };

        CollisionShapeId getShapeId() const noexcept
        {
            return shapeId_;
        };

        CollisionMode getMode() const noexcept
        {
            return mode_;
        };

        CollisionShapeType getShapeType() const noexcept
        {
            return shapeType_;
        };

    private:
        AxisAlignedBox       worldBound_;
        CollisionFilter      filter_;
        ObjectSpace::ActorId ownerActorId_;
        CollisionShapeId     shapeId_;
        CollisionMode        mode_;
        CollisionShapeType   shapeType_;

        void commitWorldBound(const AxisAlignedBox& worldBound) noexcept;

        friend class CollisionWorld;
    };
} // namespace Uncarved::CollisionSpace
