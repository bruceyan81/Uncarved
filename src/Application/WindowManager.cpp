#include "WindowManager.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <utility>

namespace Uncarved::ApplicationSpace
{
    WindowManager::WindowManager(const WindowSize& windowSize, const std::string& windowTitle)
        : windowSize_(windowSize)
        , windowTitle_(windowTitle)
    {
    }

    WindowManager::~WindowManager()
    {
        SDL_DestroyWindow(window_);
    }

    bool WindowManager::initializeWindow()
    {
        window_ = SDL_CreateWindow(windowTitle_.c_str(), windowSize_.width_, windowSize_.height_, 0);

        if (window_ == nullptr)
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
            return false;
        }

        return true;
    }
} // namespace Uncarved::ApplicationSpace
