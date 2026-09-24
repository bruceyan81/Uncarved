#pragma once

#include "Platform/Renderer.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>

#include <optional>

namespace Uncarved::PlatformSpace
{
    class Window;

    struct Renderer::Impl final
    {
        Window&       outWindow_;
        SDL_Renderer* renderer_{nullptr};
        Color         clearColor_{};

        explicit Impl(Window& outWindow);
        ~Impl();

        bool initialize();
        void setClearColor(const Color& color);
        bool clear();
        bool renderTexture(const Texture& texture);
        bool renderTexture(const Texture& texture, const SpriteDrawTransform& transform);
        bool present();
        std::optional<glm::ivec2> getViewportPixels() const;

        SDL_Renderer* getRenderer() const noexcept;
    };
} // namespace Uncarved::PlatformSpace
