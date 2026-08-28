#pragma once

#include <cstdint>
#include <string_view>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace Uncarved::ViewSpace
{
    struct Color final
    {
        std::uint8_t r_{};
        std::uint8_t g_{};
        std::uint8_t b_{};
    };

    class Renderer
    {
    public:
        Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&& other) noexcept;
        Renderer& operator=(Renderer&&) noexcept = delete;

        ~Renderer();

        bool initializeRenderer(SDL_Window* window, std::uint8_t r, std::uint8_t g, std::uint8_t b);

        bool clear();

        bool present();

        SDL_Renderer* getRenderer() noexcept
        {
            return renderer_;
        }

        bool renderTexture(SDL_Texture* texture);

        bool renderTexture(SDL_Texture* texture, float screenX, float screenY);

    private:
        SDL_Renderer* renderer_{nullptr};
        Color         clearColor_{};

        void shutdown() noexcept;
    };
} // namespace Uncarved::ViewSpace
