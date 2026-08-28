#include "TextRenderer.h"

#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <utility>

namespace Uncarved::ViewSpace
{
    TextRenderer::TextRenderer(TextRenderer&& other) noexcept
        : textEngine_(std::exchange(other.textEngine_, nullptr))
        , text_(std::exchange(other.text_, nullptr))
        , outFont_(std::exchange(other.outFont_, nullptr))
    {
    }

    TextRenderer::~TextRenderer()
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

        outFont_ = nullptr;
    }

    bool TextRenderer::initializeTextEngine(SDL_Renderer* renderer, TTF_Font* font)
    {
        textEngine_ = TTF_CreateRendererTextEngine(renderer);

        if (textEngine_ == nullptr)
        {
            std::cerr << "TTF_CreateRendererTextEngine failed: " << SDL_GetError() << '\n';
            return false;
        }

        outFont_ = font;

        return true;
    }

    bool TextRenderer::drawText(const std::string& text, float x, float y)
    {
        if (!createText(text))
        {
            return false;
        }

        return TTF_DrawRendererText(text_, x, y);
    }

    bool TextRenderer::createText(const std::string& text)
    {
        TTF_Text* createdText = TTF_CreateText(textEngine_, outFont_, text.c_str(), 0);

        if (createdText == nullptr)
        {
            std::cerr << "TTF_CreateText failed: " << SDL_GetError() << '\n';
            return false;
        }

        if (!TTF_SetTextColor(createdText, 0, 0, 0, 255))
        {
            std::cerr << "TTF_SetTextColor failed: " << SDL_GetError() << '\n';
            TTF_DestroyText(createdText);
            return false;
        }

        if (text_ != nullptr)
        {
            TTF_DestroyText(text_);
        }

        text_ = createdText;

        return true;
    }
} // namespace Uncarved::ViewSpace
