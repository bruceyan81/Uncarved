#include "Game.h"

#include "Content/GameContentLoader.h"
#include "Input/Command.h"
#include "Input/Input.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <variant>
#include <vector>

namespace Uncarved::GameSpace
{
    GameCore::GameCore(
        GameConfig&&                     gameConfig,
        SimulationCore&&                 simulationCore,
        InputSpace::InputCore&           outInputCore,
        PlatformSpace::Renderer&         outRenderer,
        PlatformSpace::TextRenderer&     outTextRenderer,
        PlatformSpace::TextureStore&     outTextureStore,
        ContentSpace::GameContentLoader& outGameContentLoader
    )
        : gameConfig_(std::move(gameConfig))
        , simulationCore_(std::move(simulationCore))
        , outInputCore_(outInputCore)
        , outRenderer_(outRenderer)
        , outTextRenderer_(outTextRenderer)
        , outTextureStore_(outTextureStore)
        , outGameContentLoader_(outGameContentLoader)
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
    }

    int GameCore::initializeGame()
    {
        const auto& initialSceneName = outGameContentLoader_.getGameConfig().initialSceneName_;

        const auto& initialResult = initializeSceneResource(initialSceneName);

        if (!initialResult.isSucceeded())
        {
            std::cerr << initialResult.getErrorMessage();
            return 1;
        }

        world_.loadActors(outGameContentLoader_.getDefinitionalActors());

        this->outGameContentLoader_.releaseLoadData();

        gameState_.initialize(gameConfig_.health_, gameConfig_.score_);

        std::vector<std::string> actorTexturePaths{};

        const auto& actors = world_.getActors();

        actorTexturePaths.reserve(actors.size());

        for (auto& actor : world_.getActors())
        {
            const auto& viewTextureName = actor.getViewTextureName();

            if (!viewTextureName.has_value())
            {
                continue;
            }

            std::string path = outGameContentLoader_.createActorTexturePath(*viewTextureName);

            if (!path.empty())
            {
                actorTexturePaths.push_back(path);
                outTextureStore_.registerActorTexturePath(std::string{*viewTextureName}, std::move(path));
            }
        }

        const auto& loadActorTextureResult = outTextureStore_.loadActorTextures(actorTexturePaths);

        if (!loadActorTextureResult.isSucceeded())
        {
            std::cerr << loadActorTextureResult.getErrorMessage();
            return 1;
        }

        return 0;
    }

    ContentSpace::ContentResult GameCore::initializeSceneResource(std::string_view sceneName)
    {
        const auto& checkResult = this->outGameContentLoader_.checkSceneResource(sceneName);

        if (!checkResult.isSucceeded())
        {
            return checkResult;
        }

        const auto& loadResult = this->outGameContentLoader_.loadSceneResource(sceneName);

        if (!loadResult.isSucceeded())
        {
            return loadResult;
        }

        return {};
    }

    int GameCore::unloadScene()
    {
        world_.clearWorld();

        return 0;
    }

    int GameCore::runGame()
    {
        const std::size_t imageCount = outTextureStore_.getIntroTextureCount();
        const std::size_t textCount = gameConfig_.introText_.size();
        const std::size_t introStepCount = std::max(imageCount, textCount);

        if (introStepCount == 0)
        {
            setGameFlowState(GameFlowState::Gameplay);
        }
        else
        {
            setGameFlowState(GameFlowState::Intro);
        }

        std::size_t introStep = 0;

        while (this->gamePhase_ == GamePhase::RunGame)
        {
            switch (getGameFlowState())
            {
                case GameFlowState::Intro:
                {
                    if (!this->outRenderer_.clear())
                    {
                        return 1;
                    }

                    switch (this->outInputCore_.pollInputRequest())
                    {
                        case InputSpace::Intention::Quit:
                            commandBuffer_.submit(QuitCommand{});
                            commitCommands();
                            break;

                        case InputSpace::Intention::AdvanceIntro:
                            if (introStep + 1 >= introStepCount)
                            {
                                setGameFlowState(GameFlowState::Gameplay);
                            }
                            else
                            {
                                introStep++;
                            }
                            break;
                    }

                    if (getGameFlowState() != GameFlowState::Intro || gamePhase_ != GamePhase::RunGame)
                    {
                        break;
                    }

                    if (imageCount > 0)
                    {
                        const std::size_t imageIndex = std::min(introStep, imageCount - 1);

                        const Texture* texture = outTextureStore_.getIntroTexture(imageIndex);

                        if (texture == nullptr)
                        {
                            return 1;
                        }

                        if (!this->outRenderer_.renderTexture(*texture))
                        {
                            return 1;
                        }
                    }

                    if (textCount > 0)
                    {
                        const std::size_t textIndex = std::min(introStep, textCount - 1);

                        if (!this->outTextRenderer_.drawText(gameConfig_.introText_[textIndex], 75.0f, 75.0f))
                        {
                            return 1;
                        }
                    }

                    if (!this->outRenderer_.present())
                    {
                        return 1;
                    }
                }
                break;
                case GameFlowState::Gameplay:
                    const InputSpace::Intention intention = outInputCore_.pollInputRequest();

                    simulationCore_.update(world_, intention, commandBuffer_);

                    commitCommands();

                    if (gamePhase_ != GamePhase::RunGame)
                    {
                        break;
                    }

                    if (!this->outRenderer_.clear())
                    {
                        return 1;
                    }

                    for (const auto& actor : world_.getActors())
                    {
                        const auto& actorTextureName = actor.getViewTextureName();

                        if (!actorTextureName.has_value())
                        {
                            continue;
                        }

                        const Texture* texture = outTextureStore_.getActorTextureByName(*actorTextureName);

                        if (texture == nullptr)
                        {
                            return 1;
                        }

                        if (!outRenderer_.renderTexture(
                                *texture,
                                PlatformSpace::SpriteTransform
                                {
                                    actor.getNormalizedPivot(),
                                    actor.getPosition(),
                                    actor.getScale(),
                                    actor.getRotationRadians()
                                }
                            ))
                        {
                            return 1;
                        }
                    }

                    if (!this->outRenderer_.present())
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

    ContentSpace::ContentResult GameCore::processSceneTransition(std::string_view nextSceneName)
    {
        const auto& initialResult = initializeSceneResource(nextSceneName);

        if (!initialResult.isSucceeded())
        {
            return initialResult;
        }

        unloadScene();

        world_.loadActors(outGameContentLoader_.getDefinitionalActors());

        this->outGameContentLoader_.releaseLoadData();

        return {};
    }

    void GameCore::commitCommands() noexcept
    {
        for (const auto& command : commandBuffer_.getCommands())
        {
            std::visit(
                [this](const auto& targetCommand)
                {
                    this->commitCommand(targetCommand);
                },
                command
            );
        }

        commandBuffer_.clearCommands();
    }

    void GameCore::commitCommand(const MoveActorCommand& command) noexcept
    {
        world_.moveActorBy(command.actorId_, command.delta_);
    }

    void GameCore::commitCommand(const SetActorVelocityCommand& command) noexcept
    {
        world_.setActorVelocity(command.actorId_, command.velocity_);
    }

    void GameCore::commitCommand(const QuitCommand& command) noexcept
    {
        (void)command;

        gamePhase_ = GamePhase::EndGame;
    }
} // namespace Uncarved::GameSpace
