#pragma once

#include "Platform/Audio/AudioDevice.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace Uncarved::PlatformSpace
{
    struct AudioDevice::Impl final
    {
        MIX_Mixer* mixer_{nullptr};
        bool       bIsMixInitialized_{false};
        bool       bHasMixerDevice_{false};

        ~Impl();

        bool initialize();
        void shutdown();

        MIX_Mixer* getMixer() const noexcept;
    };
} // namespace Uncarved::PlatformSpace
