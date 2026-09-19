#pragma once

#include <memory>

namespace Uncarved::PlatformSpace
{
    class SoundWaveStore;

    class AudioDevice
    {
    public:
        AudioDevice();

        AudioDevice(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;

        AudioDevice(AudioDevice&&) noexcept = delete;
        AudioDevice& operator=(AudioDevice&&) noexcept = delete;

        ~AudioDevice();

        bool initialize();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        friend class SoundSource;
        friend class SoundWaveStore;
    };
} // namespace Uncarved::PlatformSpace
