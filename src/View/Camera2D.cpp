#include "Camera2D.h"

#include <cmath>

namespace Uncarved::ViewSpace
{
    std::optional<Camera2D> Camera2D::createCamera2D(float orthoWidth) noexcept
    {
        if (!std::isfinite(orthoWidth) || orthoWidth <= 0.0f)
        {
            return std::nullopt;
        }

        return Camera2D{orthoWidth};
    }

    Camera2D::Camera2D(float orthoWidth) : orthoWidth_(orthoWidth)
    {
    }

    const glm::fvec2& Camera2D::getPosition() const noexcept
    {
        return position_;
    }

    void Camera2D::setPosition(const glm::fvec2& position) noexcept
    {
        position_ = position;
    }

    float Camera2D::getOrthoWidth() const noexcept
    {
        return orthoWidth_;
    }

    bool Camera2D::trySetOrthoWidth(float orthoWidth) noexcept
    {
        if (!std::isfinite(orthoWidth) || orthoWidth <= 0.0f)
        {
            return false;
        }
        else
        {
            orthoWidth_ = orthoWidth;
            return true;
        }
    }

    void Camera2D::moveBy(const glm::fvec2& delta) noexcept
    {
        position_.x += delta.x;
        position_.y += delta.y;
    }

    float Camera2D::getViewportPixelsPerWorldUnit(int viewportWidthPixels) const noexcept
    {
        return viewportWidthPixels / orthoWidth_;
    }

    glm::fvec2 Camera2D::projectWorldToViewport(
        const glm::fvec2& worldPosition,
        const glm::ivec2& viewportSizePixels
    ) const noexcept
    {
        float viewportPixelsPerWorldUnit = getViewportPixelsPerWorldUnit(viewportSizePixels.x);

        glm::fvec2 cameraRelativePosition = worldPosition - position_;

        glm::fvec2 viewportPixelOffset = cameraRelativePosition * viewportPixelsPerWorldUnit;

        return {
            viewportPixelOffset.x + viewportSizePixels.x * 0.5f,
            viewportPixelOffset.y + viewportSizePixels.y * 0.5f
        };
    }
} // namespace Uncarved::ViewSpace
