#pragma once

#include "GameConfig.h"
#include "GameSimulation.h"
#include "GameState.h"
#include "Interaction.h"

#include "Content/DataDefinition.h"
#include "Input/Input.h"
#include "Object/ActorDefinition.h"
#include "View/Camera.h"
#include "View/Renderer.h"

#include <string_view>
#include <vector>

namespace Uncarved::ContentSpace
{
    class GameContentLoader;
}

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

    class GameCore
    {
    public:
        GameCore(
            SimulationCore&&                 simulationCore,
            InteractionCore&&                interactionCore,
            InputSpace::InputCore&&          inputCore,
            ViewSpace::Renderer&&            rendererCore,
            ViewSpace::CameraManager&&       cameraManager,
            GameConfig&&                     gameConfig,
            ContentSpace::GameContentLoader& gameContentLoader
        );

        GameCore(const GameCore&) = delete;
        GameCore& operator=(const GameCore&) = delete;

        GameCore(GameCore&&) noexcept = delete;
        GameCore& operator=(GameCore&&) noexcept = delete;

        ~GameCore() = default;

        int launch();

    private:
        GamePhase                gamePhase_{GamePhase::None};
        SimulationCore           simulationCore_;
        InteractionCore          interactionCore_;
        InputSpace::InputCore    inputCore_;
        ViewSpace::Renderer      rendererCore_;
        ViewSpace::CameraManager cameraManager_;
        GameConfig               gameConfig_;
        GameState                gameState_{};

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
