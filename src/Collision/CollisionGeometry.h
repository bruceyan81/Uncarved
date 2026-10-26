#pragma once

namespace Uncarved::CollisionSpace
{
    struct AxisAlignedBox;

    bool overlapsAxisAlignedBoxes(const AxisAlignedBox& left, const AxisAlignedBox& right) noexcept;

    bool isValidAxisAlignedBox(const AxisAlignedBox& box) noexcept;
} // namespace Uncarved::CollisionSpace
