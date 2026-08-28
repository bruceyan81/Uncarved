#pragma once

#include <string>

struct TTF_Font;

namespace Uncarved::ContentSpace
{
    namespace Definition
    {
        class ResourceLoadResult;
    }

    class FontLoader final
    {
    public:
        FontLoader() = default;

        FontLoader(const FontLoader&) = delete;
        FontLoader& operator=(const FontLoader&) = delete;

        FontLoader(FontLoader&&) noexcept;
        FontLoader& operator=(FontLoader&& other) noexcept = delete;

        ~FontLoader();

        ContentSpace::Definition::ResourceLoadResult loadFont(const std::string& fontPath);

        TTF_Font* getFont() noexcept
        {
            return font_;
        }

    private:
        TTF_Font* font_{nullptr};
    };
} // namespace Uncarved::ContentSpace
