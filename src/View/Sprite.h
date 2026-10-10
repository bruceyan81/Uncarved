#pragma once

#include <glm/glm.hpp>

#include <cmath>
#include <optional>
#include <string>

namespace Uncarved::ViewSpace
{
    struct SpriteRegion final
    {
        int x_{};
        int y_{};
        int width_{};
        int height_{};
    };

    /**
     * @brief Represents a two-dimensional visual representation based on a texture.
     *
     * @member textureName
     * Name of the texture used by this sprite.
     *
     * @member texturePixelsPerWorldUnit
     * Number of texture pixels corresponding to one world unit.
     *
     * @member normalizedPivot
     * Pivot of the sprite in normalized sprite coordinates.
     * Each component is in the inclusive range [0, 1], with (0, 0) at the
     * top-left corner of the sprite and (1, 1) at the bottom-right corner.
     * The pivot defines the sprite's local origin and rotation center.
     */
    class Sprite final
    {
    public:
        static std::optional<Sprite> createSprite(
            const std::string& textureName,
            float              texturePixelsPerWorldUnit,
            const glm::fvec2&  normalizedPivot
        ) noexcept
        {
            if (
                textureName.empty()
                || !std::isfinite(texturePixelsPerWorldUnit) || texturePixelsPerWorldUnit <= 0.0f
                || !std::isfinite(normalizedPivot.x) || !std::isfinite(normalizedPivot.y)
                || 0.0f > normalizedPivot.x || 0.0f > normalizedPivot.y
                || 1.0f < normalizedPivot.x || 1.0f < normalizedPivot.y
                )
            {
                return std::nullopt;
            }

            return Sprite{textureName, texturePixelsPerWorldUnit, normalizedPivot, std::nullopt};
        }

        static std::optional<Sprite> createSprite(
            const std::string&  textureName,
            float               texturePixelsPerWorldUnit,
            const glm::fvec2&   normalizedPivot,
            const SpriteRegion& spriteRegion
        ) noexcept
        {
            if (
                textureName.empty()
                || !std::isfinite(texturePixelsPerWorldUnit) || texturePixelsPerWorldUnit <= 0.0f
                || !std::isfinite(normalizedPivot.x) || !std::isfinite(normalizedPivot.y)
                || 0.0f > normalizedPivot.x || 0.0f > normalizedPivot.y
                || 1.0f < normalizedPivot.x || 1.0f < normalizedPivot.y
                || spriteRegion.x_ < 0 || spriteRegion.y_ < 0
                || spriteRegion.width_ <= 0 || spriteRegion.height_ <= 0
                )
            {
                return std::nullopt;
            }

            return Sprite{textureName, texturePixelsPerWorldUnit, normalizedPivot, spriteRegion};
        }

        const std::string& getTextureName() const noexcept
        {
            return textureName_;
        }

        float getTexturePixelsPerWorldUnit() const noexcept
        {
            return texturePixelsPerWorldUnit_;
        }

        const glm::fvec2& getNormalizedPivot() const noexcept
        {
            return normalizedPivot_;
        }

        std::optional<SpriteRegion>& getSpriteRegion() noexcept
        {
            return spriteRegion_;
        }

        const std::optional<SpriteRegion>& getSpriteRegion() const noexcept
        {
            return spriteRegion_;
        }

    private:
        Sprite(
            const std::string& textureName,
            float              texturePixelsPerWorldUnit,
            const glm::fvec2&  normalizedPivot,
            std::optional<SpriteRegion> spriteRegion
        )
            : textureName_(textureName)
            , texturePixelsPerWorldUnit_(texturePixelsPerWorldUnit)
            , normalizedPivot_(normalizedPivot)
            , spriteRegion_(spriteRegion)
        {
        }

        std::string                 textureName_;
        float                       texturePixelsPerWorldUnit_;
        glm::fvec2                  normalizedPivot_{0.5f, 0.5f};
        std::optional<SpriteRegion> spriteRegion_;
    };

    class RuntimeSpriteVisual2D final
    {
    public:
        explicit RuntimeSpriteVisual2D(const Sprite* sprite = nullptr) noexcept : outSprite_(sprite)
        {
        }

        const Sprite* getSprite() const noexcept
        {
            return outSprite_;
        }

        void setSprite(const Sprite* sprite) noexcept
        {
            outSprite_ = sprite;
        }

        const glm::fvec2& getScale() const noexcept
        {
            return scale_;
        }

        void setScale(const glm::fvec2& scale) noexcept
        {
            scale_ = scale;
        }

        const glm::fvec2& getRenderOffsetPixels() const noexcept
        {
            return renderOffsetPixels_;
        }

        void setRenderOffsetPixels(const glm::fvec2& offset) noexcept
        {
            renderOffsetPixels_ = offset;
        }

    private:
        const Sprite* outSprite_{nullptr};

        glm::fvec2 scale_{1.0f, 1.0f};
        glm::fvec2 renderOffsetPixels_{0.0f, 0.0f};
    };
} // namespace Uncarved::ViewSpace
