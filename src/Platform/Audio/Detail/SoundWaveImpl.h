#pragma once

#include "Platform/Audio/SoundWave.h"

#include <SDL3_mixer/SDL_mixer.h>

namespace Uncarved
{
    struct SoundWave::Impl final
    {
        MIX_Audio* soundWave_{nullptr};

        ~Impl();

        MIX_Audio* getSoundWave() const noexcept;
    };
}
