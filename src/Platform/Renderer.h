#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <numbers>
#include <optional>

namespace Uncarved
{
    class Texture;
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    class TextureStore;
    class TextRenderer;
    class Window;

    struct SpriteDrawTransform
    {
        glm::fvec2 normalizedPivotPoint_{};
        glm::fvec2 screenPositionPixels_{};
        glm::fvec2 screenSizePixels_{1.0f, 1.0f};
        double     rotationRadians_{};

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

    class Renderer final
    {
    public:
        explicit Renderer(Window& outWindow);

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        Renderer(Renderer&& other) noexcept = delete;
        Renderer& operator=(Renderer&&) noexcept = delete;

        ~Renderer();

        bool initialize();

        void setClearColor(const Color& color);

        bool clear();

        bool renderTexture(const Texture& texture);

        bool renderTexture(const Texture& texture, const SpriteDrawTransform& spriteTransform);

        bool present();

        std::optional<glm::ivec2> getViewportPixels() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        friend class TextureStore;
        friend class TextRenderer;
    };
} // namespace Uncarved::PlatformSpace
