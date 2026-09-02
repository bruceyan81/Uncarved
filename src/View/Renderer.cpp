#include "Renderer.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <utility>

namespace Uncarved::ViewSpace
{
    Renderer::Renderer(Renderer&& other) noexcept
        : renderer_(std::exchange(other.renderer_, nullptr))
        , clearColor_(other.clearColor_)
    {
    }

    Renderer::~Renderer()
    {
        shutdown();
    }

    bool Renderer::initializeRenderer(SDL_Window* window, std::uint8_t r, std::uint8_t g, std::uint8_t b)
    {
        renderer_ = SDL_CreateRenderer(window, nullptr);

        if (renderer_ == nullptr)
        {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
            return false;
        }

        clearColor_ = {r, g, b};

        if (!SDL_SetRenderVSync(renderer_, 1))
        {
            std::cerr << "SDL_SetRenderVSync failed: " << SDL_GetError() << '\n';
            shutdown();
            return false;
        }

        return true;
    }

    bool Renderer::clear()
    {
        if (!SDL_SetRenderDrawColor(renderer_, clearColor_.r_, clearColor_.g_, clearColor_.b_, SDL_ALPHA_OPAQUE))
        {
            return false;
        }

        return SDL_RenderClear(renderer_);
    }

    bool Renderer::present()
    {
        return SDL_RenderPresent(renderer_);
    }

    bool Renderer::renderTexture(SDL_Texture* texture)
    {
        if (renderer_ == nullptr || texture == nullptr)
        {
            return false;
        }

        return SDL_RenderTexture(renderer_, texture, nullptr, nullptr);
    }

    bool Renderer::renderTexture(SDL_Texture* texture, const SpriteTransform& spriteTransform)
    {
        if (renderer_ == nullptr || texture == nullptr)
        {
            return false;
        }

        const float textureWidth = spriteTransform.scale_.x * (texture->w);
        const float textureHeight = spriteTransform.scale_.y * (texture->h);

        SDL_FRect dstRect{spriteTransform.position_.x, spriteTransform.position_.y, textureWidth, textureHeight};

        const auto normalizedPivot = spriteTransform.normalizedPivot_;

        SDL_FPoint rotationCenter{};

        if (normalizedPivot.has_value())
        {
            rotationCenter.x = textureWidth * (*normalizedPivot).x;
            rotationCenter.y = textureHeight * (*normalizedPivot).y;
        }

        return SDL_RenderTextureRotated(
            renderer_,
            texture,
            nullptr,
            &dstRect,
            spriteTransform.getRotationDegrees(),
            normalizedPivot ? &rotationCenter : nullptr,
            SDL_FLIP_NONE
        );
    }

    SpriteTransform Renderer::createSpriteTransform(
        const std::optional<glm::fvec2>& normalizedPivot,
        const glm::fvec2&                position,
        const glm::fvec2                 scale,
        double                           rotationRadians
    ) const
    {
        std::optional<SDL_FPoint> normalizedPivotPoint = std::nullopt;

        if (normalizedPivot.has_value())
        {
            normalizedPivotPoint = {(*normalizedPivot).x, (*normalizedPivot).y};
        }

        return {normalizedPivotPoint, {position.x, position.y}, {scale.x, scale.y}, rotationRadians};
    }

    void Renderer::shutdown() noexcept
    {
        if (renderer_ != nullptr)
        {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
    }
} // namespace Uncarved::ViewSpace
