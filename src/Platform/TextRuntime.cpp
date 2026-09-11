#include "TextRuntime.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <memory>

namespace Uncarved::PlatformSpace
{
    struct TextRuntime::Impl final
    {
        bool bIsSDLTtfInitialized_{false};

        ~Impl()
        {
            if (bIsSDLTtfInitialized_)
            {
                TTF_Quit();
                bIsSDLTtfInitialized_ = false;
            }
        }

        bool initialize()
        {
            if (bIsSDLTtfInitialized_)
            {
                return true;
            }

            bIsSDLTtfInitialized_ = TTF_Init();

            if (!bIsSDLTtfInitialized_)
            {
                std::cerr << "TTF_Init failed: " << SDL_GetError() << '\n';
                return false;
            }

            return true;
        }
    };

    TextRuntime::TextRuntime() : impl_(std::make_unique<Impl>())
    {
    }

    TextRuntime::~TextRuntime() = default;

    bool TextRuntime::initialize()
    {
        return impl_->initialize();
    }
} // namespace Uncarved::PlatformSpace
