#pragma once

#include <glm/glm.hpp>

#include <cmath>
#include <optional>
#include <string>
#include <utility>

namespace Uncarved::ViewSpace
{
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

            return Sprite{textureName, texturePixelsPerWorldUnit, normalizedPivot};
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

    private:
        Sprite(const std::string& textureName, float texturePixelsPerWorldUnit, const glm::fvec2& normalizedPivot)
            : textureName_(textureName)
            , texturePixelsPerWorldUnit_(texturePixelsPerWorldUnit)
            , normalizedPivot_(normalizedPivot)
        {
        }

        std::string textureName_;
        float       texturePixelsPerWorldUnit_;
        glm::fvec2  normalizedPivot_{0.5f, 0.5f};
    };
} // namespace Uncarved::ViewSpace
