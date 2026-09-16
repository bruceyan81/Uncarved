#pragma once

#include "CommandBuffer.h"
#include "GameConfig.h"
#include "GameSimulation.h"
#include "GameState.h"
#include "World.h"

#include "Content/ContentResult.h"
#include "Input/Input.h"
#include "Platform/Renderer.h"
#include "Platform/TextRenderer.h"
#include "Platform/TextureStore.h"

#include <string_view>

namespace Uncarved
{
    namespace ContentSpace
    {
        class GameContentLoader;
    }

    namespace TimeSpace
    {
        class AppTime;
    }
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

    class GameCore
    {
    public:
        GameCore(
            GameConfig&&     gameConfig,
            SimulationCore&& simulationCore,

            TimeSpace::AppTime&              outAppTime,
            InputSpace::InputCore&           outInputCore,
            PlatformSpace::Renderer&         outRenderer,
            PlatformSpace::TextRenderer&     outTextRenderer,
            PlatformSpace::TextureStore&     outTextureStore,
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

        void setGameFlowState(GameFlowState gameFlowState) noexcept
        {
            gameFlowState_ = gameFlowState;
        }

    private:
        GamePhase     gamePhase_{GamePhase::None};
        GameFlowState gameFlowState_{GameFlowState::None};
        GameState     gameState_{};
        World         world_{};
        CommandBuffer commandBuffer_{};

        GameConfig     gameConfig_;
        SimulationCore simulationCore_;

        TimeSpace::AppTime&              outAppTime_;
        InputSpace::InputCore&           outInputCore_;
        PlatformSpace::Renderer&         outRenderer_;
        PlatformSpace::TextRenderer&     outTextRenderer_;
        PlatformSpace::TextureStore&     outTextureStore_;
        ContentSpace::GameContentLoader& outGameContentLoader_;

        int initializeGame();

        ContentSpace::ContentResult initializeSceneResource(std::string_view sceneName);

        int unloadScene();

        int runGame();

        int endGame();

        ContentSpace::ContentResult processSceneTransition(std::string_view nextSceneName);

        void commitCommands() noexcept;

        void commitCommand(const MoveActorCommand& command) noexcept;

        void commitCommand(const SetActorVelocityCommand& command) noexcept;

        void commitCommand(const QuitCommand& command) noexcept;
    };
} // namespace Uncarved::GameSpace
