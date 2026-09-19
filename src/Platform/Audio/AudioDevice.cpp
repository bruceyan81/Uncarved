#include "AudioDevice.h"
#include "Detail/AudioDeviceImpl.h"

#include <SDL3_mixer/SDL_mixer.h>

#include <iostream>

namespace Uncarved::PlatformSpace
{
    AudioDevice::Impl::~Impl()
    {
        shutdown();
    }

    void AudioDevice::Impl::shutdown()
    {
        if (bHasMixerDevice_ && mixer_ != nullptr)
        {
            MIX_DestroyMixer(mixer_);
            mixer_ = nullptr;
            bHasMixerDevice_ = false;
        }

        if (bIsMixInitialized_)
        {
            MIX_Quit();
            bIsMixInitialized_ = false;
        }
    }

    bool AudioDevice::Impl::initialize()
    {
        if (!bIsMixInitialized_)
        {
            if (!MIX_Init())
            {
                std::cerr << "MIX_Init failed: " << SDL_GetError() << '\n';
                return false;
            }
            else
            {
                bIsMixInitialized_ = true;
            }
        }

        if (bHasMixerDevice_ && mixer_ != nullptr)
        {
            return true;
        }

        mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        if (mixer_ == nullptr)
        {
            shutdown();
            return false;
        }

        bHasMixerDevice_ = true;
        return true;
    }

    MIX_Mixer* AudioDevice::Impl::getMixer() const noexcept
    {
        return mixer_;
    }

    AudioDevice::AudioDevice() : impl_(std::make_unique<Impl>())
    {
    }

    AudioDevice::~AudioDevice() = default;

    bool AudioDevice::initialize()
    {
        return impl_->initialize();
    }
} // namespace Uncarved::PlatformSpace
