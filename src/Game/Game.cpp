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

        if (!world_.tryLoadActors(gameContentLoader_.getDefinitionalActors()))
        {
            return 1;
        }

        this->gameContentLoader_.releaseLoadData();

        gameState_.health_ = gameConfig_.health_;
        gameState_.score_ = gameConfig_.score_;

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
        world_.clearWorld();

        interactionCore_.clearResults();

        return 0;
    }

    int GameCore::runGame()
    {
        if (this->imageLoader_.getTextureCount() <= 0)
        {
            setGameFlowState(GameFlowState::Gameplay);
        }
        else
        {
            setGameFlowState(GameFlowState::Intro);
        }

        std::size_t textureIndex = 0;

        while (this->gamePhase_ == GamePhase::RunGame)
        {
            switch (getGameFlowState())
            {
                case GameFlowState::Intro:
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
                                setGameFlowState(GameFlowState::Gameplay);
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
                case GameFlowState::Gameplay:

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

    ContentSpace::Definition::ResourceLoadResult GameCore::processSceneTransition(std::string_view nextSceneName)
    {
        const auto& initialResult = initializeSceneResource(nextSceneName);

        if (!initialResult.isSucceeded())
        {
            return initialResult;
        }

        unloadScene();

        if (!world_.tryLoadActors(gameContentLoader_.getDefinitionalActors()))
        {
            return {ContentSpace::Definition::ResourceLoadError::InvalidActor, "Actor's index out of bounds"};
        }

        this->gameContentLoader_.releaseLoadData();

        return {};
    }
} // namespace Uncarved::GameSpace
