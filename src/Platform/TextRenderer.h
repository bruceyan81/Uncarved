#pragma once

#include <iosfwd>
#include <memory>

namespace Uncarved
{
    class Font;
}

namespace Uncarved::PlatformSpace
{
    class Renderer;
    class TextRuntime;

    class TextRenderer final
    {
    public:
        TextRenderer(TextRuntime& outTextRuntime, Font& outFont, Renderer& outRenderer);

        ~TextRenderer();

        bool initialize();

        bool drawText(const std::string& text, float x, float y);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
