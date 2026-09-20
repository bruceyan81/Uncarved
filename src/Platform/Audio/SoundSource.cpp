#include "SoundSource.h"

#include "AudioDevice.h"
#include "Detail/AudioDeviceImpl.h"
#include "Detail/SoundWaveImpl.h"
#include "SoundWave.h"

#include <SDL3_mixer/SDL_mixer.h>

#include <iostream>

namespace Uncarved::PlatformSpace
{
    struct SoundSource::Impl final
    {
        AudioDevice& outAudioDevice_;
        MIX_Track*   mixTrack_{nullptr};

        explicit Impl(AudioDevice& outAudioDevice) : outAudioDevice_(outAudioDevice)
        {
        }

        ~Impl()
        {
            MIX_DestroyTrack(mixTrack_);
            mixTrack_ = nullptr;
        }

        bool initialize()
        {
            auto* mixer = outAudioDevice_.impl_->getMixer();

            if (mixer != nullptr)
            {
                mixTrack_ = MIX_CreateTrack(mixer);

                if (mixTrack_ == nullptr)
                {
                    std::cerr << "MIX_Track failed: " << SDL_GetError() << '\n';
                    MIX_DestroyTrack(mixTrack_);
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                std::cerr << "Mixer is nullptr." << '\n';
                return false;
            }
        }

        bool setSoundWave(SoundWave& soundWave)
        {
            return MIX_SetTrackAudio(mixTrack_, soundWave.impl_->getSoundWave());
        }

        bool play()
        {
            return MIX_PlayTrack(mixTrack_, 0);
        }

        bool pause()
        {
            return MIX_PauseTrack(mixTrack_);
        }

        bool resume()
        {
            return MIX_ResumeTrack(mixTrack_);
        }

        bool stop()
        {
            return MIX_StopTrack(mixTrack_, 0);
        }

        bool isPlaying() const noexcept
        {
            return MIX_TrackPlaying(mixTrack_);
        }

        bool isPaused() const noexcept
        {
            return MIX_TrackPaused(mixTrack_);
        }
    };

    SoundSource::SoundSource(AudioDevice& outAudioDevice) : impl_(std::make_unique<Impl>(outAudioDevice))
    {
    }

    SoundSource::~SoundSource() = default;

    bool SoundSource::initialize()
    {
        return impl_->initialize();
    }

    bool SoundSource::setSoundWave(SoundWave& soundWave)
    {
        return impl_->setSoundWave(soundWave);
    }

    bool SoundSource::play()
    {
        return impl_->play();
    }

    bool SoundSource::pause()
    {
        return impl_->pause();
    }

    bool SoundSource::resume()
    {
        return impl_->resume();
    }

    bool SoundSource::stop()
    {
        return impl_->stop();
    }

    bool SoundSource::isPlaying() const noexcept
    {
        return impl_->isPlaying();
    }

    bool SoundSource::isPaused() const noexcept
    {
        return impl_->isPaused();
    }

} // namespace Uncarved::PlatformSpace
