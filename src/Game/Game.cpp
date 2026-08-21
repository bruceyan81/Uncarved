#include "Game.h"

#include "GameCommon.h"
#include "Gameplay.h"

#include "Content/DataDefinition.h"
#include "Content/GameContentLoader.h"
#include "Input/Input.h"
#include "Object/Actor.h"

#include <cstdlib>
#include <utility>

namespace Uncarved::GameSpace
{
    GameCore::GameCore(
        SimulationCore&&                 simulationCore,
        InteractionCore&&                interactionCore,
        InputSpace::InputCore&&          inputCore,
        ViewSpace::Renderer&&            rendererCore,
        GameConfig&&                     gameConfig,
        ContentSpace::GameContentLoader& gameContentLoader
    )
        : simulationCore_(std::move(simulationCore))
        , interactionCore_(std::move(interactionCore))
        , inputCore_(std::move(inputCore))
        , rendererCore_(std::move(rendererCore))
        , gameConfig_(std::move(gameConfig))
        , gameContentLoader_(gameContentLoader)
    {
        gamePhase_ = GamePhase::InitGame;
    }

    int GameCore::launch()
    {
        if (this->gamePhase_ == GamePhase::InitGame)
        {
            if (this->initializeGame() == 0)
            {
                this->gamePhase_ = GamePhase::RunGame;
                if (this->runGame() == 0)
                {
                    if (this->gamePhase_ == GamePhase::EndGame)
                    {
                        return this->endGame();
                    }
                    else
                    {
                        return 1;
                    }
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
        return 0;
    }

    int GameCore::initializeGame()
    {
        const auto& initialSceneIt = this->gameContentLoader_.getGameConfig().find("initial_scene");

        const auto& initialSceneName = std::get<std::string>(initialSceneIt->second);

        const auto& initialResult = initializeSceneResource(initialSceneName);

        if (!initialResult.isSucceeded())
        {
            initialResult.showMessage();
            return 1;
        }

        const auto& loadSceneActorResult = this->loadSceneActors(this->gameContentLoader_.getDefinitionalActors());

        if (!loadSceneActorResult.isSucceeded())
        {
            loadSceneActorResult.showMessage();
            return 1;
        }

        this->gameContentLoader_.releaseLoadData();

        this->gameState_.updateHealth(this->gameConfig_.health_);
        this->gameState_.updateScore(this->gameConfig_.score_);

        updateGameState();

        return 0;
    }

    ContentSpace::Definition::ResourceLoadResult GameCore::initializeSceneResource(std::string_view sceneName)
    {
        const auto& checkResult = this->gameContentLoader_.checkSceneResource(sceneName);

        if (!checkResult.isSucceeded())
        {
            return checkResult;
        }

        const auto& loadResult = this->gameContentLoader_.loadSceneResource(sceneName);

        if (!loadResult.isSucceeded())
        {
            return loadResult;
        }

        return {};
    }

    int GameCore::unloadScene()
    {
        GameState& gameState = this->gameState_;

        gameState.actors_.clear();
        gameState.actorIndexById_.clear();
        gameState.playerIndex_.reset();
        gameState.worldBuffer_.fill(' ');
        gameState.npcOccupancyGrid_.fill(0);
        gameState.blockingOccupancyGrid_.fill(0);
        gameState.request_ = std::nullopt;

        InteractionCore& interactionCore = this->interactionCore_;

        interactionCore.clearResults();

        return 0;
    }

    int GameCore::runGame()
    {
        while (this->gamePhase_ == GamePhase::RunGame)
        {
            if (this->inputCore_.pollQuitRequest())
            {
                this->gamePhase_ = GamePhase::EndGame;
                break;
            }

            if (!this->rendererCore_.clear())
            {
                return 1;
            }

            if (!this->rendererCore_.present())
            {
                return 1;
            }
        }

        return 0;
    }

    int GameCore::endGame()
    {
        return 0;
    }

    ContentSpace::Definition::ResourceLoadResult
    GameCore::loadSceneActors(const std::vector<ObjectSpace::ActorDefinition>& actorDefinitions)
    {
        this->gameState_.actors_.reserve(this->gameState_.actors_.size() + actorDefinitions.size());

        for (const auto& definition : actorDefinitions)
        {
            if (definition.x_ >= 0 && definition.x_ < kMapWidth && definition.y_ >= 0 && definition.y_ < kMapHeight)
            {
                const char view = definition.view_.empty() ? '?' : definition.view_.front();

                this->gameState_.actors_.emplace_back(
                    definition.bBlocking_,
                    view,
                    glm::ivec2{definition.x_, definition.y_},
                    glm::ivec2{definition.velX_, definition.velY_},
                    definition.actorName_,
                    definition.nearbyDialogue_,
                    definition.contactDialogue_
                );

                const std::size_t actorIndex = this->gameState_.actors_.size() - 1;
                const auto&       actor = this->gameState_.actors_.back();

                this->gameState_.actorIndexById_[actor.getId()] = actorIndex;

                if (actor.getActorName() == "player")
                {
                    this->gameState_.playerIndex_ = actorIndex;
                }
            }
            else
            {
                return {ContentSpace::Definition::ResourceLoadError::InvalidActor, "Actor's index out of bounds"};
            }
        }

        return {};
    }

    void GameCore::updateGameState() noexcept
    {
        const ObjectSpace::Actor* playerPtr = this->gameState_.getPlayer();

        for (const auto& actor : this->gameState_.actors_)
        {
            const glm::ivec2 position = actor.getPosition();

            if (!playerPtr || actor.getId() != playerPtr->getId())
            {
                this->gameState_.npcOccupancyGrid_[position.y * kMapWidth + position.x]++;
            }

            if (actor.getBlocking())
            {
                this->gameState_.blockingOccupancyGrid_[position.y * kMapWidth + position.x]++;
            }
        }
    }

    ContentSpace::Definition::ResourceLoadResult GameCore::processSceneTransition(std::string_view nextSceneName)
    {
        const auto& initialResult = initializeSceneResource(nextSceneName);

        if (!initialResult.isSucceeded())
        {
            return initialResult;
        }

        unloadScene();

        const auto& loadSceneActorResult = this->loadSceneActors(this->gameContentLoader_.getDefinitionalActors());

        if (!loadSceneActorResult.isSucceeded())
        {
            return loadSceneActorResult;
        }

        this->gameContentLoader_.releaseLoadData();

        updateGameState();

        return {};
    }
} // namespace Uncarved::GameSpace
