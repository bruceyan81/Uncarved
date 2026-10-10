#pragma once

#include "Sprite.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace Uncarved::ViewSpace
{
    class SpriteCatalog final
    {
    public:
        explicit SpriteCatalog(const std::unordered_map<std::string, Sprite>& sprites) noexcept : outSprites_(sprites)
        {
        }

        const Sprite* findSprite(std::string_view name) const noexcept
        {
            const auto it = outSprites_.find(std::string{name});

            if (it == outSprites_.end())
            {
                return nullptr;
            }

            return &it->second;
        }

    private:
        const std::unordered_map<std::string, Sprite>& outSprites_;
    };
} // namespace Uncarved::ViewSpace
