#include "SoundWave.h"
#include "Detail/SoundWaveImpl.h"

#include <SDL3_mixer/SDL_mixer.h>

#include <utility>

namespace Uncarved
{
    SoundWave::Impl::~Impl()
    {
        MIX_DestroyAudio(soundWave_);
        soundWave_ = nullptr;
    }

    MIX_Audio* SoundWave::Impl::getSoundWave() const noexcept
    {
        return soundWave_;
    }

    SoundWave::SoundWave(std::unique_ptr<SoundWave::Impl> impl) : impl_(std::move(impl))
    {
    }

    SoundWave::SoundWave(SoundWave&& other) noexcept : impl_(std::move(other.impl_))
    {
    }

    SoundWave& SoundWave::operator=(SoundWave&& other) noexcept
    {
        if (this != &other)
        {
            impl_ = std::move(other.impl_);
        }

        return *this;
    }

    SoundWave::~SoundWave() = default;
} // namespace Uncarved
