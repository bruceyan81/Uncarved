#pragma once

#include <glm/glm.hpp>

#include <cmath>
#include <optional>
#include <string>
#include <utility>

namespace Uncarved::ViewSpace
{
    class Sprite final
    {
    public:
        static std::optional<Sprite> createSprite(
            const std::string& textureName,
            float              pixelsPerWorldUnit,
            const glm::fvec2&  normalizedPivot
        ) noexcept
        {
            if (textureName.empty()
                || !std::isfinite(pixelsPerWorldUnit)
                || pixelsPerWorldUnit <= 0.0f
                || !std::isfinite(normalizedPivot.x)
                || !std::isfinite(normalizedPivot.y))
            {
                return std::nullopt;
            }

            return Sprite{textureName, pixelsPerWorldUnit, normalizedPivot};
        }

        const std::string& getTextureName() const noexcept
        {
            return textureName_;
        }

        float getPixelsPerWorldUnit() const noexcept
        {
            return pixelsPerWorldUnit_;
        }

        const glm::fvec2& getNormalizedPivot() const noexcept
        {
            return normalizedPivot_;
        }

    private:
        Sprite(const std::string& textureName, float pixelsPerWorldUnit, const glm::fvec2& normalizedPivot)
            : textureName_(textureName)
            , pixelsPerWorldUnit_(pixelsPerWorldUnit)
            , normalizedPivot_(normalizedPivot)
        {
        }

        std::string textureName_;
        float       pixelsPerWorldUnit_;
        glm::fvec2  normalizedPivot_{0.5f, 0.5f};
    };
} // namespace Uncarved::ViewSpace
