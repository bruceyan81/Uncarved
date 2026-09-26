#pragma once

#include "CommandBuffer.h"
#include "GameConfig.h"
#include "GameSimulation.h"
#include "GameState.h"
#include "World.h"

#include "View/Camera2D.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace Uncarved
{
    class SoundWave;

    namespace ContentSpace
    {
        struct ContentResult;

        class GameContentLoader;
    } // namespace ContentSpace

    namespace InputSpace
    {
        class InputCore;
    }

    namespace TimeSpace
    {
        class AppTime;
    }

    namespace PlatformSpace
    {
        class Renderer;
        class SoundSource;
        class SoundWaveStore;
        class TextRenderer;
        class TextureStore;
    } // namespace PlatformSpace

    namespace ViewSpace
    {
        class SceneRenderer;
    } // namespace ViewSpace

} // namespace Uncarved

namespace Uncarved::GameSpace
{
    enum class GamePhase
    {
        None,
        InitGame,
        RunGame,
        EndGame,
        Count
    };

    enum class GameFlowState
    {
        None,
        Intro,
        Gameplay
    };

    class GameCore final
    {
    public:
        GameCore(
            GameConfig&&          gameConfig,
            SimulationCore&&      simulationCore,
            ViewSpace::Camera2D&& camera2d,

            TimeSpace::AppTime&              outAppTime,
            InputSpace::InputCore&           outInputCore,
            PlatformSpace::Renderer&         outRenderer,
            PlatformSpace::TextRenderer&     outTextRenderer,
            PlatformSpace::TextureStore&     outTextureStore,
            ViewSpace::SceneRenderer&        outSceneRenderer,
            PlatformSpace::SoundSource&      outSoundSource,
            PlatformSpace::SoundWaveStore&   outSoundWaveStore,
            ContentSpace::GameContentLoader& outGameContentLoader
        );

        GameCore(const GameCore&) = delete;
        GameCore& operator=(const GameCore&) = delete;

        GameCore(GameCore&&) noexcept = delete;
        GameCore& operator=(GameCore&&) noexcept = delete;

        ~GameCore() = default;

        int launch();

        GameFlowState getGameFlowState() const noexcept
        {
            return gameFlowState_;
        }

        bool transitionGameFlowState(GameFlowState gameFlowState);

    private:
        GamePhase     gamePhase_{GamePhase::None};
        GameFlowState gameFlowState_{GameFlowState::None};
        GameState     gameState_{};
        World         world_{};
        CommandBuffer commandBuffer_{};

        GameConfig          gameConfig_;
        SimulationCore      simulationCore_;
        ViewSpace::Camera2D camera2d_;

        TimeSpace::AppTime&              outAppTime_;
        InputSpace::InputCore&           outInputCore_;
        PlatformSpace::Renderer&         outRenderer_;
        PlatformSpace::TextRenderer&     outTextRenderer_;
        PlatformSpace::TextureStore&     outTextureStore_;
        ViewSpace::SceneRenderer&        outSceneRenderer_;
        PlatformSpace::SoundSource&      outSoundSource_;
        PlatformSpace::SoundWaveStore&   outSoundWaveStore_;
        ContentSpace::GameContentLoader& outGameContentLoader_;

        std::vector<SoundWave*> outIntroSoundWaves_{};
        std::vector<SoundWave*> outGameplaySoundWaves_{};

        const std::vector<SoundWave*>* activeSoundWaves_{nullptr};
        std::size_t                    activeSoundWaveIndex_{};

        int initializeGame();

        bool updateSoundPlayback();

        bool playActiveSoundWave();

        ContentSpace::ContentResult initializeSceneResource(std::string_view sceneName);

        int runGame();

        int endGame();

        ContentSpace::ContentResult processSceneTransition(std::string_view nextSceneName);

        ContentSpace::ContentResult loadWorldTextures(const World& world);

        void commitCommands() noexcept;

        void commitCommand(const MoveActorCommand& command) noexcept;

        void commitCommand(const SetActorVelocityCommand& command) noexcept;

        void commitCommand(const QuitCommand& command) noexcept;
    };
} // namespace Uncarved::GameSpace
