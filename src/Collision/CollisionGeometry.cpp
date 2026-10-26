#include "CollisionGeometry.h"

#include "CollisionTypes.h"

#include <cmath>

namespace Uncarved::CollisionSpace
{
    bool overlapsAxisAlignedBoxes(const AxisAlignedBox& left, const AxisAlignedBox& right) noexcept
    {
        return (left.max_.x > right.min_.x && left.min_.x < right.max_.x)
            && (left.max_.y > right.min_.y && left.min_.y < right.max_.y);
    }

    bool isValidAxisAlignedBox(const AxisAlignedBox& box) noexcept
    {
        return std::isfinite(box.max_.x) && std::isfinite(box.max_.y)
            && std::isfinite(box.min_.x) && std::isfinite(box.min_.y)
            && box.max_.x > box.min_.x && box.max_.y > box.min_.y;
    }
} // namespace Uncarved::CollisionSpace
