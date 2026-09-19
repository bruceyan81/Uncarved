#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Uncarved
{
    class SoundWave;

    namespace ContentSpace
    {
        struct ContentResult;
    }
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    class AudioDevice;

    class SoundWaveStore final
    {
    public:
        explicit SoundWaveStore(AudioDevice& outAudioDevice);
        ~SoundWaveStore();

        ContentSpace::ContentResult loadSoundWaves(const std::vector<std::string>& soundWavePaths);
        ContentSpace::ContentResult
        loadSoundWaves(const std::vector<std::string>& soundWavePaths, std::vector<SoundWave*>& outSoundWaves);
        const SoundWave* getSoundWaveByPath(const std::string& soundWavePath) const noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
