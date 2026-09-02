#pragma once

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <cstdint>
#include <numbers>
#include <optional>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

namespace Uncarved::ViewSpace
{
    struct SpriteTransform
    {
        std::optional<SDL_FPoint> normalizedPivot_{};
        SDL_FPoint                position_{};
        SDL_FPoint                scale_{1.0f, 1.0f};
        double                    rotationRadians_{};

        double getRotationDegrees() const noexcept
        {
            return rotationRadians_ * 180.0 / std::numbers::pi;
        }
    };

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

        bool renderTexture(SDL_Texture* texture, const SpriteTransform& spriteTransform);

        SpriteTransform createSpriteTransform(
            const std::optional<glm::fvec2>& normalizedPivot,
            const glm::fvec2&                position,
            const glm::fvec2                 scale,
            double                           rotationRadians
        ) const;

    private:
        SDL_Renderer* renderer_{nullptr};
        Color         clearColor_{};

        void shutdown() noexcept;
    };
} // namespace Uncarved::ViewSpace
