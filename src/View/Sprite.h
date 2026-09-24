#pragma once

#include <cmath>
#include <optional>
#include <string>
#include <utility>

namespace Uncarved::ViewSpace
{
    class Sprite final
    {
    public:
        static std::optional<Sprite> createSprite(std::string&& textureName, float pixelsPerWorldUnit) noexcept
        {
            if (textureName.empty() || !std::isfinite(pixelsPerWorldUnit) || pixelsPerWorldUnit <= 0.0f)
            {
                return std::nullopt;
            }

            return Sprite{std::move(textureName), pixelsPerWorldUnit};
        }

        const std::string& getTextureName() const noexcept
        {
            return textureName_;
        }

        float getPixelsPerWorldUnit() const noexcept
        {
            return pixelsPerWorldUnit_;
        }

    private:
        Sprite(std::string&& textureName, float pixelsPerWorldUnit)
            : textureName_(std::move(textureName))
            , pixelsPerWorldUnit_(pixelsPerWorldUnit)
        {
        }

        std::string textureName_;
        float       pixelsPerWorldUnit_;
    };
} // namespace Uncarved::ViewSpace
