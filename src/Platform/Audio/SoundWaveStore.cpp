#include "SoundWaveStore.h"

#include "AudioDevice.h"
#include "Detail/AudioDeviceImpl.h"
#include "Detail/SoundWaveImpl.h"
#include "SoundWave.h"

#include "Content/ContentResult.h"

#include <SDL3_mixer/SDL_mixer.h>

#include <unordered_map>
#include <utility>

namespace Uncarved::PlatformSpace
{
    struct SoundWaveStore::Impl final
    {
        AudioDevice& outAudioDevice_;

        using SoundWaveCache = std::unordered_map<std::string, SoundWave>;

        SoundWaveCache soundWaveCache_;

        Impl(AudioDevice& outAudioDevice) : outAudioDevice_(outAudioDevice)
        {
        }

        ~Impl() = default;

        ContentSpace::ContentResult loadSoundWaves(const std::vector<std::string>& soundWavePaths)
        {
            std::vector<SoundWave*> ignoredSoundWaves{};
            return loadSoundWaves(soundWavePaths, ignoredSoundWaves);
        }

        ContentSpace::ContentResult
        loadSoundWaves(const std::vector<std::string>& soundWavePaths, std::vector<SoundWave*>& outSoundWaves)
        {
            std::vector<SoundWave*> loadedSoundWaves{};
            loadedSoundWaves.reserve(soundWavePaths.size());

            SoundWaveCache loadedSoundWaveCache{};

            for (const auto& soundWavePath : soundWavePaths)
            {
                if (soundWaveCache_.contains(soundWavePath) || loadedSoundWaveCache.contains(soundWavePath))
                {
                    continue;
                }

                auto soundWaveImpl = std::make_unique<SoundWave::Impl>(nullptr);
                soundWaveImpl->soundWave_ =
                    MIX_LoadAudio(outAudioDevice_.impl_->getMixer(), soundWavePath.c_str(), true);

                SoundWave soundWave{std::move(soundWaveImpl)};

                if (soundWave.impl_->getSoundWave() == nullptr)
                {
                    loadedSoundWaveCache.clear();

                    return {ContentSpace::ResourceError{
                        "error: failed to load audio " + soundWavePath + ": " + SDL_GetError(),
                        ContentSpace::ResourceErrorType::LoadFailed
                    }};
                }

                loadedSoundWaveCache.emplace(soundWavePath, std::move(soundWave));
            }

            soundWaveCache_.reserve(soundWaveCache_.size() + loadedSoundWaveCache.size());

            soundWaveCache_.merge(loadedSoundWaveCache);

            for (const auto& soundWavePath : soundWavePaths)
            {
                const auto soundWaveIt = soundWaveCache_.find(soundWavePath);

                if (soundWaveIt == soundWaveCache_.end())
                {
                    return {ContentSpace::ResourceError{
                        "error: failed to find loaded audio " + soundWavePath,
                        ContentSpace::ResourceErrorType::LoadFailed
                    }};
                }

                loadedSoundWaves.push_back(&(soundWaveIt->second));
            }

            outSoundWaves = std::move(loadedSoundWaves);

            return {};
        }

        const SoundWave* getSoundWaveByPath(const std::string& soundWavePath) const noexcept
        {
            if (soundWavePath.empty())
            {
                return nullptr;
            }

            if (auto it = soundWaveCache_.find(soundWavePath); it != soundWaveCache_.end())
            {
                return &(it->second);
            }
            else
            {
                return nullptr;
            }
        }
    };

    SoundWaveStore::SoundWaveStore(AudioDevice& outAudioDevice) : impl_(std::make_unique<Impl>(outAudioDevice))
    {
    }

    SoundWaveStore::~SoundWaveStore()
    {
    }

    ContentSpace::ContentResult SoundWaveStore::loadSoundWaves(const std::vector<std::string>& soundWavePaths)
    {
        return impl_->loadSoundWaves(soundWavePaths);
    }

    ContentSpace::ContentResult SoundWaveStore::loadSoundWaves(
        const std::vector<std::string>& soundWavePaths,
        std::vector<SoundWave*>&        outSoundWaves
    )
    {
        return impl_->loadSoundWaves(soundWavePaths, outSoundWaves);
    }

    const SoundWave* SoundWaveStore::getSoundWaveByPath(const std::string& soundWavePath) const noexcept
    {
        return impl_->getSoundWaveByPath(soundWavePath);
    }
} // namespace Uncarved::PlatformSpace
