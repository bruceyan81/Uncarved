#pragma once

#include "CommandBuffer.h"
#include "GameConfig.h"
#include "GameSimulation.h"
#include "GameState.h"
#include "World.h"

#include "Content/DataDefinition.h"
#include "Content/ImageLoader.h"
#include "Input/Input.h"
#include "View/Renderer.h"
#include "View/TextRenderer.h"

#include <string_view>

namespace Uncarved::ContentSpace
{
    class GameContentLoader;
} // namespace Uncarved::ContentSpace

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
            SimulationCore&&            simulationCore,
            InputSpace::InputCore&&     inputCore,
            ViewSpace::Renderer&&       rendererCore,
            ViewSpace::TextRenderer&&   textRendererCore,
            GameConfig&&                gameConfig,
            ContentSpace::ImageLoader&& imageLoader,

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

        SimulationCore            simulationCore_;
        InputSpace::InputCore     inputCore_;
        ViewSpace::Renderer       rendererCore_;
        ViewSpace::TextRenderer   textRendererCore_;
        GameConfig                gameConfig_;
        World                     world_{};
        ContentSpace::ImageLoader imageLoader_;
        CommandBuffer             commandBuffer_{};

        ContentSpace::GameContentLoader& outGameContentLoader_;

        int initializeGame();

        ContentSpace::Definition::ResourceLoadResult initializeSceneResource(std::string_view sceneName);

        int unloadScene();

        int runGame();

        int endGame();

        ContentSpace::Definition::ResourceLoadResult processSceneTransition(std::string_view nextSceneName);

        void commitCommands() noexcept;

        void commitCommand(const MoveActorCommand& command) noexcept;

        void commitCommand(const SetActorVelocityCommand& command) noexcept;

        void commitCommand(const QuitCommand& command) noexcept;
    };
} // namespace Uncarved::GameSpace
