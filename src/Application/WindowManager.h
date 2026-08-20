#pragma once

#include <string>

struct SDL_Window;

namespace Uncarved::ApplicationSpace
{
    struct WindowSize final
    {
        int width_{};
        int height_{};
    };

    class WindowManager
    {
    private:
        SDL_Window* window_{nullptr};
        WindowSize  windowSize_{};
        std::string windowTitle_;

    public:
        WindowManager(const std::string& windowTitle, const WindowSize& windowSize);

        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        WindowManager(WindowManager&&) = delete;
        WindowManager& operator=(WindowManager&&) noexcept = delete;

        ~WindowManager();

        bool initializeWindow();

        SDL_Window* getWindow() const noexcept
        {
            return window_;
        }
    };
} // namespace Uncarved::ApplicationSpace
