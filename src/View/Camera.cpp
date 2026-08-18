#include "Camera.h"

#include <algorithm>

namespace Uncarved::ViewSpace
{
    CameraManager::CameraManager(
        int         viewportWidth,
        int         viewportHeight,
        std::size_t defaultViewportWidth,
        std::size_t defaultViewportHeight
    )
        : viewportWidth_(viewportWidth > 0 ? static_cast<std::size_t>(viewportWidth) : defaultViewportWidth)
        , viewportHeight_(viewportHeight > 0 ? static_cast<std::size_t>(viewportHeight) : defaultViewportHeight)
        , presentationBuffer_(viewportWidth_ * viewportHeight_, ' ')
    {
    }

    void CameraManager::followActorByActorPosition(
        const char*       worldBuffer,
        const std::size_t worldWidth,
        const std::size_t worldHeight,
        const glm::ivec2& targetActorPosition
    ) noexcept
    {
        std::fill(this->presentationBuffer_.begin(), this->presentationBuffer_.end(), ' ');

        const int viewportOriginInWorldX = targetActorPosition.x - static_cast<int>(viewportWidth_) / 2;
        const int viewportOriginInWorldY = targetActorPosition.y - static_cast<int>(viewportHeight_) / 2;

        for (std::size_t y = 0; y < viewportHeight_; ++y)
        {
            const int worldY = viewportOriginInWorldY + static_cast<int>(y);
            if (worldY < 0 || worldY >= worldHeight)
            {
                continue;
            }

            for (std::size_t x = 0; x < viewportWidth_; ++x)
            {
                const int worldX = viewportOriginInWorldX + static_cast<int>(x);
                if (worldX < 0 || worldX >= worldWidth)
                {
                    continue;
                }

                this->presentationBuffer_[y * viewportWidth_ + x] = worldBuffer[worldY * worldWidth + worldX];
            }
        }
    }
} // namespace Uncarved::ViewSpace
