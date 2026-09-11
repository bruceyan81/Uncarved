#pragma once

#include "Platform/Renderer.h"

#include <SDL3/SDL.h>

namespace Uncarved::PlatformSpace
{
    class Window;

    struct Renderer::Impl final
    {
        explicit Impl(Window& outWindow);
        ~Impl();

        bool initialize();
        void setClearColor(const Color& color);
        bool clear();
        bool renderTexture(const Texture& texture);
        bool renderTexture(const Texture& texture, const SpriteTransform& transform);
        bool present();

        SDL_Renderer* getRenderer() const noexcept;

        Window&       outWindow_;
        SDL_Renderer* renderer_{nullptr};
        Color         clearColor_{};
    };
} // namespace Uncarved::PlatformSpace
