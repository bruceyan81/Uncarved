#include "Transform2D.h"

#include <cmath>

namespace Uncarved::ObjectSpace
{
    bool isValidTransform2D(const Transform2D& transform2d) noexcept
    {
        return std::isfinite(transform2d.position_.x) && std::isfinite(transform2d.position_.y)
            && std::isfinite(transform2d.scale_.x) && std::isfinite(transform2d.scale_.y)
            && std::isfinite(transform2d.rotationRadians_);
    }
} // namespace Uncarved::ObjectSpace
