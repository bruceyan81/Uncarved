#include "Detail/RendererImpl.h"
#include "Font.h"
#include "FontLoader.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "TextRuntime.h"

#include "Content/ContentResult.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace Uncarved
{
    struct Font::Impl final
    {
        TTF_Font* font_{nullptr};

        explicit Impl(TTF_Font* fontPtr) : font_(fontPtr)
        {
        }

        TTF_Font* getFont() const noexcept
        {
            return font_;
        }

        ~Impl()
        {
            if (font_ != nullptr)
            {
                TTF_CloseFont(font_);
                font_ = nullptr;
            }
        }
    };

    Font::Font(std::unique_ptr<Font::Impl> impl) : impl_(std::move(impl))
    {
    }

    Font::Font(Font&& other) noexcept : impl_(std::move(other.impl_))
    {
    }

    Font& Font::operator=(Font&& other) noexcept
    {
        if (this != &other)
        {
            impl_ = std::move(other.impl_);
        }

        return *this;
    }

    Font::~Font() = default;
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    namespace
    {
        constexpr float kFontPtSize = 64.0f;
    }

    struct FontLoader::Impl final
    {
        TextRuntime& outTextRuntime_;
        Font         font_{nullptr};

        explicit Impl(TextRuntime& outTextRuntime) : outTextRuntime_(outTextRuntime)
        {
        }

        ContentSpace::ContentResult loadFont(const std::string& fontPath)
        {
            auto fontImpl = std::make_unique<Font::Impl>(nullptr);
            fontImpl->font_ = TTF_OpenFont(fontPath.c_str(), kFontPtSize);

            Font loadedFont{std::move(fontImpl)};

            if (loadedFont.impl_->getFont() == nullptr)
            {
                return {ContentSpace::ResourceError{
                    "error: failed to load font " + fontPath + ": " + SDL_GetError(),
                    ContentSpace::ResourceErrorType::LoadFailed
                }};
            }

            font_ = std::move(loadedFont);

            return {};
        }

        Font& getFont() noexcept
        {
            return font_;
        }
    };

    FontLoader::FontLoader(TextRuntime& outTextRuntime) : impl_(std::make_unique<Impl>(outTextRuntime))
    {
    }

    FontLoader::~FontLoader() = default;

    ContentSpace::ContentResult FontLoader::loadFont(const std::string& fontPath)
    {
        return impl_->loadFont(fontPath);
    }

    Font& FontLoader::getFont() noexcept
    {
        return impl_->getFont();
    }
} // namespace Uncarved::PlatformSpace

namespace Uncarved::PlatformSpace
{
    struct TextRenderer::Impl final
    {
        TextRuntime& outTextRuntime_;
        Font&        outFont_;
        Renderer&    outRenderer_;

        TTF_Text*       text_{};
        TTF_TextEngine* textEngine_{nullptr};

        Impl(TextRuntime& outTextRuntime, Font& outFont, Renderer& outRenderer)
            : outTextRuntime_(outTextRuntime)
            , outFont_(outFont)
            , outRenderer_(outRenderer)
        {
        }

        ~Impl()
        {
            shutdown();
        }

        bool initialize()
        {
            if (textEngine_ != nullptr)
            {
                return true;
            }

            textEngine_ = TTF_CreateRendererTextEngine(outRenderer_.impl_->getRenderer());

            if (textEngine_ == nullptr)
            {
                std::cerr << "TTF_CreateRendererTextEngine failed: " << SDL_GetError() << '\n';
                return false;
            }

            return true;
        }

        bool createText(const std::string& text)
        {
            if (text_ != nullptr)
            {
                TTF_DestroyText(text_);
                text_ = nullptr;
            }

            text_ = TTF_CreateText(textEngine_, outFont_.impl_->getFont(), text.c_str(), 0);

            if (text_ == nullptr)
            {
                std::cerr << "TTF_CreateText failed: " << SDL_GetError() << '\n';
                return false;
            }

            if (!TTF_SetTextColor(text_, 0, 0, 0, 255))
            {
                std::cerr << "TTF_SetTextColor failed: " << SDL_GetError() << '\n';
                TTF_DestroyText(text_);
                text_ = nullptr;
                return false;
            }

            return true;
        }

        bool drawText(const std::string& text, float x, float y)
        {
            if (!createText(text))
            {
                return false;
            }

            return TTF_DrawRendererText(text_, x, y);
        }

        void shutdown()
        {
            if (text_ != nullptr)
            {
                TTF_DestroyText(text_);
                text_ = nullptr;
            }

            if (textEngine_ != nullptr)
            {
                TTF_DestroyRendererTextEngine(textEngine_);
                textEngine_ = nullptr;
            }
        }
    };

    TextRenderer::TextRenderer(TextRuntime& outTextRuntime, Font& outFont, Renderer& outRenderer)
        : impl_(std::make_unique<Impl>(outTextRuntime, outFont, outRenderer))
    {
    }

    TextRenderer::~TextRenderer() = default;

    bool TextRenderer::initialize()
    {
        return impl_->initialize();
    }

    bool TextRenderer::drawText(const std::string& text, float x, float y)
    {
        return impl_->drawText(text, x, y);
    }
} // namespace Uncarved::PlatformSpace
