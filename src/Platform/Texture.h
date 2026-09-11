#pragma once

#include <memory>

namespace Uncarved
{
    namespace PlatformSpace
    {
        class Renderer;
        class TextureStore;
    } // namespace PlatformSpace

    class Texture final
    {
    public:
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

        int getWidth() const noexcept;

        int getHeight() const noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        explicit Texture(std::unique_ptr<Impl> impl);

        friend class PlatformSpace::TextureStore;
        friend class PlatformSpace::Renderer;
    };
} // namespace Uncarved
