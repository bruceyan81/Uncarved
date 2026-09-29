#pragma once

#include <glm/glm.hpp>

#include <optional>

namespace Uncarved::ViewSpace
{
    class Camera2D final
    {
    public:
        static std::optional<Camera2D> createCamera2D(float orthoWidth) noexcept;

        Camera2D(const Camera2D&) = delete;
        Camera2D& operator=(const Camera2D&) = delete;

        Camera2D(Camera2D&&) noexcept = default;
        Camera2D& operator=(Camera2D&&) noexcept = default;

        ~Camera2D() = default;

        const glm::fvec2& getPosition() const noexcept;
        void              setPosition(const glm::fvec2& position) noexcept;

        float getOrthoWidth() const noexcept;
        bool  trySetOrthoWidth(float orthoWidth) noexcept;

        void moveBy(const glm::fvec2& delta) noexcept;

        /**
         * @brief Returns the number of viewport pixels per world unit.
         *
         * @param viewportWidthPixels
         * Width of the viewport, in pixels.
         *
         * @return
         * The number of viewport pixels per world unit.
         */
        float getViewportPixelsPerWorldUnit(int viewportWidthPixels) const noexcept;

        /**
         * @brief Transforms a position from world space to viewport space.
         *
         * A viewport is a rectangular region of the renderer output onto
         * which a camera projects a two-dimensional image.
         *
         * Viewport space is measured in pixels, with its origin at the top-left corner.
         * Its positive X axis points right, and its positive Y axis points down.
         *
         * @param worldPosition
         * Position in world space, in world units.
         *
         * @param viewportSizePixels
         * Width and height of the viewport, in pixels.
         *
         * @return
         * The corresponding position in viewport space, in pixels.
         */
        glm::fvec2
        worldToViewport(const glm::fvec2& worldPosition, const glm::ivec2& viewportSizePixels) const noexcept;

    private:
        explicit Camera2D(float orthoWidth);

        /**
         * @brief Position of the camera in world space.
         *
         * The camera position maps to the center of the viewport.
         */
        glm::fvec2 position_{};

        /**
         * @brief Width of the orthographic view, in world units.
         */
        float orthoWidth_;
    };
} // namespace Uncarved::ViewSpace
