#pragma once

#include <string>

struct SDL_Renderer;
struct TTF_TextEngine;
struct TTF_Text;
struct TTF_Font;

namespace Uncarved::ViewSpace
{
    class TextRenderer
    {
    public:
        TextRenderer() = default;

        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;

        TextRenderer(TextRenderer&& other) noexcept;
        TextRenderer& operator=(TextRenderer&&) noexcept = delete;

        ~TextRenderer();

        bool initializeTextEngine(SDL_Renderer* renderer, TTF_Font* font);

        bool drawText(const std::string& text, float x, float y);

    private:
        TTF_TextEngine* textEngine_{nullptr};
        TTF_Text*       text_{nullptr};
        TTF_Font*       outFont_{nullptr};

        bool createText(const std::string& text);
    };
} // namespace Uncarved::ViewSpace
