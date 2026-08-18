#pragma once

#include <cstddef>
#include <string_view>

namespace Uncarved::ViewSpace
{
    class Renderer
    {
    public:
        void renderText(std::string_view message, bool bIsNewLine);

        void renderWorld(const char* worldBuffer, std::size_t viewportWidth, std::size_t viewportHeight);
    };
} // namespace Uncarved::ViewSpace
