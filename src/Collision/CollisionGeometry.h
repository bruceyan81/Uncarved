#pragma once

#include <glm/glm.hpp>

namespace Uncarved::CollisionSpace
{
    struct AxisAlignedBox;

    bool overlapsAxisAlignedBoxes(const AxisAlignedBox& left, const AxisAlignedBox& right) noexcept;

    bool isValidCollisionTransform(const glm::fvec2& position, const glm::fvec2& scale) noexcept;

    bool isValidAxisAlignedBox(const AxisAlignedBox& box) noexcept;
} // namespace Uncarved::CollisionSpace
