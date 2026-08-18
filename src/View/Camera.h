#pragma once

#include <glm/glm.hpp>

#include <cstddef>
#include <vector>

namespace Uncarved::ViewSpace
{
    class CameraManager
    {
    public:
        CameraManager(
            int         viewportWidth,
            int         viewportHeight,
            std::size_t defaultViewportWidth,
            std::size_t defaultViewportHeight
        );

        CameraManager(const CameraManager&) = delete;
        CameraManager& operator=(const CameraManager&) = delete;

        CameraManager(CameraManager&&) noexcept = default;
        CameraManager& operator=(CameraManager&&) noexcept = default;

        ~CameraManager() = default;

        std::size_t getViewportWidth() const noexcept
        {
            return viewportWidth_;
        }

        std::size_t getViewportHeight() const noexcept
        {
            return viewportHeight_;
        }

        void followActorByActorPosition(
            const char*       worldBuffer,
            const std::size_t worldWidth,
            const std::size_t worldHeight,
            const glm::ivec2& targetActorPosition
        ) noexcept;

        const char* getPresentationBuffer() const noexcept
        {
            return presentationBuffer_.data();
        }

    private:
        std::size_t       viewportWidth_;
        std::size_t       viewportHeight_;
        std::vector<char> presentationBuffer_;
    };
} // namespace Uncarved::ViewSpace
