#pragma once

#include <glm/glm.hpp>

#include <cstdint>

namespace Uncarved::CollisionSpace
{
    using CollisionShapeId = std::uint64_t;
    using CollisionMask = std::uint32_t;

    enum class CollisionCategory : CollisionMask
    {
        None = 0,
        Character = 1u << 0,
        WorldSolid = 1u << 1,
        Interactable = 1u << 2
    };

    enum class CollisionMode
    {
        Solid,
        Sensor
    };

    struct CollisionFilter final
    {
        CollisionCategory category_{CollisionCategory::None};
        CollisionMask     mask_{};
    };

    struct AxisAlignedBox
    {
        glm::fvec2 min_{};
        glm::fvec2 max_{};
    };

    enum class CollisionShapeType
    {
        AxisAlignedBox
    };

    constexpr CollisionMask makeCollisionMask(CollisionCategory category) noexcept
    {
        return static_cast<CollisionMask>(category);
    }

    constexpr bool canFiltersInteract(const CollisionFilter& leftFilter, const CollisionFilter& rightFilter) noexcept
    {
        return (leftFilter.mask_ & makeCollisionMask(rightFilter.category_)) != 0
            && (rightFilter.mask_ & makeCollisionMask(leftFilter.category_)) != 0;
    }
} // namespace Uncarved::CollisionSpace
