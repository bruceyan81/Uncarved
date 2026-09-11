#include "PlatformRuntime.h"

#include <SDL3/SDL.h>

#include <iostream>
#include <memory>

namespace Uncarved::PlatformSpace
{
    struct PlatformRuntime::Impl final
    {
        bool bIsSDLInitialized_{false};

        ~Impl()
        {
            shutdown();
        }

        bool initialize()
        {
            if (bIsSDLInitialized_)
            {
                return true;
            }

            bIsSDLInitialized_ = SDL_Init(SDL_INIT_VIDEO);

            if (!bIsSDLInitialized_)
            {
                std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
                shutdown();
                return false;
            }

            return true;
        }

        void shutdown()
        {
            if (bIsSDLInitialized_)
            {
                SDL_Quit();
                bIsSDLInitialized_ = false;
            }
        }
    };

    PlatformRuntime::PlatformRuntime() : impl_(std::make_unique<Impl>())
    {
    }

    PlatformRuntime::~PlatformRuntime() = default;

    bool PlatformRuntime::initialize()
    {
        return impl_->initialize();
    }
} // namespace Uncarved::PlatformSpace
