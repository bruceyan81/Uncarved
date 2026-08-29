#include "FontLoader.h"

#include "ContentResult.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <string>
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

    ContentResult FontLoader::loadFont(const std::string& fontPath)
    {
        TTF_Font* loadedFont = TTF_OpenFont(fontPath.c_str(), kPtSize);

        if (loadedFont == nullptr)
        {
            return {ResourceError{
                "error: failed to load font " + fontPath + ": " + SDL_GetError(),
                ResourceErrorType::LoadFailed
            }};
        }

        if (font_ != nullptr)
        {
            TTF_CloseFont(font_);
        }

        font_ = loadedFont;

        return {};
    }
} // namespace Uncarved::ContentSpace
