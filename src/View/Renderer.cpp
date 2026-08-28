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

    bool Renderer::renderTexture(SDL_Texture* texture, float screenX, float screenY)
    {
        if (renderer_ == nullptr || texture == nullptr)
        {
            return false;
        }

        SDL_FRect dstRect{
            screenX,
            screenY,
            texture->w,
            texture->h
        };

        return SDL_RenderTexture(renderer_, texture, nullptr, &dstRect);
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
