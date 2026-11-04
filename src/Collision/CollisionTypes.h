#pragma once

#include <glm/glm.hpp>

#include <bit>
#include <cstdint>

namespace Uncarved::CollisionSpace
{
    using CollisionShapeId = std::uint64_t;
    using CollisionMask = std::uint32_t;

    /**
     * @brief Strong type for a single collision category bit
     * This type keeps a single category distinct from a multi-bit CollisionMask
     */
    struct CollisionCategory final
    {
        CollisionMask bit_{};
    };

    enum class CollisionMode
    {
        Solid,
        Sensor
    };

    struct CollisionFilter final
    {
        CollisionCategory category_{};
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
        return static_cast<CollisionMask>(category.bit_);
    }

    constexpr bool isValidCollisionCategory(CollisionCategory category) noexcept
    {
        return std::has_single_bit(category.bit_);
    }

    constexpr bool canFiltersInteract(const CollisionFilter& leftFilter, const CollisionFilter& rightFilter) noexcept
    {
        return (leftFilter.mask_ & makeCollisionMask(rightFilter.category_)) != 0
            && (rightFilter.mask_ & makeCollisionMask(leftFilter.category_)) != 0;
    }
} // namespace Uncarved::CollisionSpace
