#include "FontLoader.h"

#include "Content/DataDefinition.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <utility>

namespace Uncarved::ContentSpace
{
    namespace
    {
        constexpr float kPtSize = 64.0f;
    }

    FontLoader::FontLoader(FontLoader&& other) noexcept : font_(std::exchange(other.font_, nullptr))
    {
    }

    FontLoader::~FontLoader()
    {
        if (font_ != nullptr)
        {
            TTF_CloseFont(font_);
        }

        font_ = nullptr;
    }

    ContentSpace::Definition::ResourceLoadResult FontLoader::loadFont(const std::string& fontPath)
    {
        TTF_Font* loadedFont = TTF_OpenFont(fontPath.c_str(), kPtSize);

        if (loadedFont == nullptr)
        {
            return {ContentSpace::Definition::ResourceLoadError::MissingFont, "Error reading Font."};
        }

        if (font_ != nullptr)
        {
            TTF_CloseFont(font_);
        }

        font_ = loadedFont;

        return {};
    }
} // namespace Uncarved::ContentSpace
