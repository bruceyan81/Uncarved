#pragma once

#include "GameConfig.h"
#include "GameSimulation.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "Interaction.h"

#include "Content/DataDefinition.h"
#include "Content/ImageLoader.h"
#include "Input/Input.h"
#include "Object/ActorDefinition.h"
#include "View/Renderer.h"

#include <string_view>
#include <vector>

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
            SimulationCore&&                 simulationCore,
            InteractionCore&&                interactionCore,
            InputSpace::InputCore&&          inputCore,
            ViewSpace::Renderer&&            rendererCore,
            GameConfig&&                     gameConfig,
            ContentSpace::ImageLoader&&      imageLoader,
            ContentSpace::GameContentLoader& gameContentLoader
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
        InteractionCore           interactionCore_;
        InputSpace::InputCore     inputCore_;
        ViewSpace::Renderer       rendererCore_;
        GameConfig                gameConfig_;
        GameStateManager          gameStateManager_{};
        ContentSpace::ImageLoader imageLoader_;

        ContentSpace::GameContentLoader& gameContentLoader_;

        int initializeGame();

        void updateGameState() noexcept;

        ContentSpace::Definition::ResourceLoadResult initializeSceneResource(std::string_view sceneName);

        int unloadScene();

        ContentSpace::Definition::ResourceLoadResult
        loadSceneActors(const std::vector<ObjectSpace::ActorDefinition>& actorDefinitions);

        int runGame();

        int endGame();

        ContentSpace::Definition::ResourceLoadResult processSceneTransition(std::string_view nextSceneName);
    };
} // namespace Uncarved::GameSpace
