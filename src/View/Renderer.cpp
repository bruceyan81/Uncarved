#include "Renderer.h"

#include <iostream>
#include <string>

namespace Uncarved::ViewSpace
{
    void Renderer::renderText(std::string_view message, bool bIsNewLine)
    {
        if (message.empty())
        {
            return;
        }

        if (bIsNewLine)
        {
            std::cout << message << '\n';
        }
        else
        {
            std::cout << message;
        }
    }

    void Renderer::renderWorld(const char* worldBuffer, std::size_t viewportWidth, std::size_t viewportHeight)
    {
        std::string lineBuffer;
        lineBuffer.reserve(viewportWidth);

        for (std::size_t y = 0; y < viewportHeight; ++y)
        {
            for (std::size_t x = 0; x < viewportWidth; ++x)
            {
                lineBuffer += worldBuffer[y * viewportWidth + x];
            }
            std::cout << lineBuffer << '\n';
            lineBuffer.clear();
        }
    }
} // namespace Uncarved::ViewSpace
