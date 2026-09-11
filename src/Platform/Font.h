#pragma once

#include <memory>

namespace Uncarved
{
    namespace PlatformSpace
    {
        class FontLoader;
        class TextRenderer;
    } // namespace PlatformSpace

    class Font final
    {
    public:
        Font(Font&& other) noexcept;
        Font& operator=(Font&& other) noexcept;

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        ~Font();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        explicit Font(std::unique_ptr<Impl> impl);

        friend class PlatformSpace::FontLoader;
        friend class PlatformSpace::TextRenderer;
    };
} // namespace Uncarved
