#pragma once

#include <memory>

namespace Uncarved::PlatformSpace
{
    class SoundSource;
    class SoundWaveStore;
}

namespace Uncarved
{
    class SoundWave final
    {
    public:
        SoundWave(const SoundWave&) = delete;
        SoundWave& operator=(const SoundWave&) = delete;

        SoundWave(SoundWave&&) noexcept;
        SoundWave& operator=(SoundWave&&) noexcept;

        ~SoundWave();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        explicit SoundWave(std::unique_ptr<Impl> impl);

        friend class PlatformSpace::SoundSource;
        friend class PlatformSpace::SoundWaveStore;
    };
} // namespace Uncarved
