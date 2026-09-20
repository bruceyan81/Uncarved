#pragma once

#include <memory>

namespace Uncarved
{
    class SoundWave;
}

namespace Uncarved::PlatformSpace
{
    class AudioDevice;

    class SoundSource final
    {
    public:
        explicit SoundSource(AudioDevice& outAudioDevice);
        ~SoundSource();

        bool initialize();

        bool setSoundWave(SoundWave& soundWave);

        bool play();

        bool pause();

        bool resume();

        bool stop();

        bool isPlaying() const noexcept;

        bool isPaused() const noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
