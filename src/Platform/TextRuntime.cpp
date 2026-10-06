#include "TextRuntime.h"

#include "Logging/Log.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

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
                UC_LOG(gLogPlatform, LogVerbosity::Error, "TTF_Init failed: {}", SDL_GetError());
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
