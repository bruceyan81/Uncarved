#include "Game.h"

#include "GameCommon.h"
#include "Gameplay.h"

#include "Content/DataDefinition.h"
#include "Content/GameContentLoader.h"
#include "Content/ImageLoader.h"
#include "Input/Command.h"
#include "Input/Input.h"
#include "Object/Actor.h"

#include <cstddef>
#include <utility>
#include <variant>

namespace Uncarved::GameSpace
{
    GameCore::GameCore(
        SimulationCore&&                 simulationCore,
        InteractionCore&&                interactionCore,
        InputSpace::InputCore&&          inputCore,
        ViewSpace::Renderer&&            rendererCore,
        GameConfig&&                     gameConfig,
        ContentSpace::ImageLoader&&      imageLoader,
        ContentSpace::GameContentLoader& gameContentLoader
    )
        : simulationCore_(std::move(simulationCore))
        , interactionCore_(std::move(interactionCore))
        , inputCore_(std::move(inputCore))
        , rendererCore_(std::move(rendererCore))
        , gameConfig_(std::move(gameConfig))
        , imageLoader_(std::move(imageLoader))
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
            initialResult.showErrorMessage();
            return 1;
        }

        const auto& loadSceneActorResult = this->loadSceneActors(this->gameContentLoader_.getDefinitionalActors());

        if (!loadSceneActorResult.isSucceeded())
        {
            loadSceneActorResult.showErrorMessage();
            return 1;
        }

        this->gameContentLoader_.releaseLoadData();

        this->gameStateManager_.updateHealth(this->gameConfig_.health_);
        this->gameStateManager_.updateScore(this->gameConfig_.score_);

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
        GameStateManager& gameStateManager = this->gameStateManager_;

        gameStateManager.actors_.clear();
        gameStateManager.actorIndexById_.clear();
        gameStateManager.playerIndex_.reset();
        gameStateManager.worldBuffer_.fill(' ');
        gameStateManager.npcOccupancyGrid_.fill(0);
        gameStateManager.blockingOccupancyGrid_.fill(0);
        gameStateManager.request_ = std::nullopt;

        InteractionCore& interactionCore = this->interactionCore_;

        interactionCore.clearResults();

        return 0;
    }

    int GameCore::runGame()
    {
        if (this->imageLoader_.getTextureCount() <= 0)
        {
            this->gameStateManager_.setGameState(GameState::Gameplay);
        }
        else
        {
            this->gameStateManager_.setGameState(GameState::Intro);
        }

        std::size_t textureIndex = 0;

        while (this->gamePhase_ == GamePhase::RunGame)
        {
            switch (this->gameStateManager_.getGameState())
            {
                case GameState::Intro:
                {
                    if (!this->rendererCore_.clear())
                    {
                        return 1;
                    }

                    switch (this->inputCore_.pollInputRequest())
                    {
                        case InputSpace::Intention::Quit:
                            this->gamePhase_ = GamePhase::EndGame;
                            break;

                        case InputSpace::Intention::NextImage:
                            if (textureIndex < this->imageLoader_.getTextureCount() - 1)
                            {
                                textureIndex++;
                            }
                            else
                            {
                                this->gameStateManager_.setGameState(GameState::Gameplay);
                            }
                            break;
                    }

                    auto* texturePtr = this->imageLoader_.getTexture(textureIndex);

                    if (!this->rendererCore_.renderTexture(texturePtr))
                    {
                        return 1;
                    }

                    if (!this->rendererCore_.present())
                    {
                        return 1;
                    }
                }
                break;
                case GameState::Gameplay:

                    switch (this->inputCore_.pollInputRequest())
                    {
                        case InputSpace::Intention::Quit:
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
                    break;
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
        this->gameStateManager_.actors_.reserve(this->gameStateManager_.actors_.size() + actorDefinitions.size());

        for (const auto& definition : actorDefinitions)
        {
            if (definition.x_ >= 0 && definition.x_ < kMapWidth && definition.y_ >= 0 && definition.y_ < kMapHeight)
            {
                const char view = definition.view_.empty() ? '?' : definition.view_.front();

                this->gameStateManager_.actors_.emplace_back(
                    definition.bBlocking_,
                    view,
                    glm::ivec2{definition.x_, definition.y_},
                    glm::ivec2{definition.velX_, definition.velY_},
                    definition.actorName_,
                    definition.nearbyDialogue_,
                    definition.contactDialogue_
                );

                const std::size_t actorIndex = this->gameStateManager_.actors_.size() - 1;
                const auto&       actor = this->gameStateManager_.actors_.back();

                this->gameStateManager_.actorIndexById_[actor.getId()] = actorIndex;

                if (actor.getActorName() == "player")
                {
                    this->gameStateManager_.playerIndex_ = actorIndex;
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
        const ObjectSpace::Actor* playerPtr = this->gameStateManager_.getPlayer();

        for (const auto& actor : this->gameStateManager_.actors_)
        {
            const glm::ivec2 position = actor.getPosition();

            if (!playerPtr || actor.getId() != playerPtr->getId())
            {
                this->gameStateManager_.npcOccupancyGrid_[position.y * kMapWidth + position.x]++;
            }

            if (actor.getBlocking())
            {
                this->gameStateManager_.blockingOccupancyGrid_[position.y * kMapWidth + position.x]++;
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
