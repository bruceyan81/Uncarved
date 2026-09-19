#include "Game.h"

#include "Content/GameContentLoader.h"
#include "Input/Command.h"
#include "Input/Input.h"
#include "Platform/Audio/SoundSource.h"
#include "Platform/Audio/SoundWaveStore.h"
#include "Time/AppTime.h"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <variant>
#include <vector>

namespace Uncarved::GameSpace
{
    GameCore::GameCore(
        GameConfig&&     gameConfig,
        SimulationCore&& simulationCore,

        TimeSpace::AppTime&              outAppTime,
        InputSpace::InputCore&           outInputCore,
        PlatformSpace::Renderer&         outRenderer,
        PlatformSpace::TextRenderer&     outTextRenderer,
        PlatformSpace::TextureStore&     outTextureStore,
        PlatformSpace::SoundSource&      outSoundSource,
        PlatformSpace::SoundWaveStore&   outSoundWaveStore,
        ContentSpace::GameContentLoader& outGameContentLoader
    )
        : gameConfig_(std::move(gameConfig))
        , simulationCore_(std::move(simulationCore))

        , outAppTime_(outAppTime)
        , outInputCore_(outInputCore)
        , outRenderer_(outRenderer)
        , outTextRenderer_(outTextRenderer)
        , outTextureStore_(outTextureStore)
        , outSoundSource_(outSoundSource)
        , outSoundWaveStore_(outSoundWaveStore)
        , outGameContentLoader_(outGameContentLoader)
    {
        gamePhase_ = GamePhase::InitGame;
    }

    int GameCore::launch()
    {
        if (gamePhase_ == GamePhase::InitGame)
        {
            if (initializeGame() == 0)
            {
                gamePhase_ = GamePhase::RunGame;

                if (runGame() == 0)
                {
                    if (gamePhase_ == GamePhase::EndGame)
                    {
                        return endGame();
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

    bool GameCore::transitionGameFlowState(GameFlowState gameFlowState)
    {
        if (gameFlowState_ == gameFlowState)
        {
            return true;
        }

        const std::vector<SoundWave*>* nextSoundWaves = nullptr;

        switch (gameFlowState)
        {
            case GameFlowState::Intro:
            {
                nextSoundWaves = &outIntroSoundWaves_;
            }
            break;
            case GameFlowState::Gameplay:
            {
                nextSoundWaves = &outGameplaySoundWaves_;
            }
            break;
            default:
                return false;
        }

        if (outSoundSource_.isPlaying() || outSoundSource_.isPaused())
        {
            if (!outSoundSource_.stop())
            {
                return false;
            }
        }

        activeSoundWaves_ = nextSoundWaves;
        activeSoundWaveIndex_ = 0;

        if (!activeSoundWaves_->empty() && !playActiveSoundWave())
        {
            return false;
        }

        gameFlowState_ = gameFlowState;

        return true;
    }

    bool GameCore::updateSoundPlayback()
    {
        if (activeSoundWaves_ == nullptr || activeSoundWaves_->empty())
        {
            return true;
        }

        if (outSoundSource_.isPlaying() || outSoundSource_.isPaused())
        {
            return true;
        }

        activeSoundWaveIndex_ = (activeSoundWaveIndex_ + 1) % activeSoundWaves_->size();

        return playActiveSoundWave();
    }

    bool GameCore::playActiveSoundWave()
    {
        SoundWave* soundWave = (*activeSoundWaves_)[activeSoundWaveIndex_];

        if (soundWave == nullptr)
        {
            return false;
        }

        return outSoundSource_.setSoundWave(*soundWave) && outSoundSource_.play();
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

        outGameContentLoader_.releaseLoadData();

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

        const auto& introBgmArray = outGameContentLoader_.getIntroConfig().introBgmArray_;
        std::vector<std::string> introBgmPaths{};
        introBgmPaths.reserve(introBgmArray.size());

        for (const auto& introBgmName : introBgmArray)
        {
            introBgmPaths.emplace_back(outGameContentLoader_.createIntroBgmPath(introBgmName));
        }

        const auto loadIntroBgmResult = outSoundWaveStore_.loadSoundWaves(introBgmPaths, outIntroSoundWaves_);

        if (!loadIntroBgmResult.isSucceeded())
        {
            std::cerr << loadIntroBgmResult.getErrorMessage();
            return 1;
        }

        const auto& gameplayBgmArray = outGameContentLoader_.getGameConfig().gameplayBgmArray_;
        std::vector<std::string> gameplayBgmPaths{};
        gameplayBgmPaths.reserve(gameplayBgmArray.size());

        for (const auto& gameplayBgmName : gameplayBgmArray)
        {
            gameplayBgmPaths.emplace_back(outGameContentLoader_.createGameplayBgmPath(gameplayBgmName));
        }

        const auto loadGameplayBgmResult = outSoundWaveStore_.loadSoundWaves(gameplayBgmPaths, outGameplaySoundWaves_);

        if (!loadGameplayBgmResult.isSucceeded())
        {
            std::cerr << loadGameplayBgmResult.getErrorMessage();
            return 1;
        }

        return 0;
    }

    ContentSpace::ContentResult GameCore::initializeSceneResource(std::string_view sceneName)
    {
        const auto& checkResult = outGameContentLoader_.checkSceneResource(sceneName);

        if (!checkResult.isSucceeded())
        {
            return checkResult;
        }

        const auto& loadResult = outGameContentLoader_.loadSceneResource(sceneName);

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
            if (!transitionGameFlowState(GameFlowState::Gameplay))
            {
                return 1;
            }
        }
        else
        {
            if (!transitionGameFlowState(GameFlowState::Intro))
            {
                return 1;
            }
        }

        std::size_t introStep = 0;

        while (gamePhase_ == GamePhase::RunGame)
        {
            outAppTime_.update();

            if (!updateSoundPlayback())
            {
                return 1;
            }

            switch (getGameFlowState())
            {
                case GameFlowState::Intro:
                {
                    if (!outRenderer_.clear())
                    {
                        return 1;
                    }

                    switch (outInputCore_.pollInputRequest())
                    {
                        case InputSpace::Intention::Quit:
                            commandBuffer_.submit(QuitCommand{});
                            commitCommands();
                            break;

                        case InputSpace::Intention::AdvanceIntro:
                            if (introStep + 1 >= introStepCount)
                            {
                                if (!transitionGameFlowState(GameFlowState::Gameplay))
                                {
                                    return 1;
                                }
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

                        if (!outRenderer_.renderTexture(*texture))
                        {
                            return 1;
                        }
                    }

                    if (textCount > 0)
                    {
                        const std::size_t textIndex = std::min(introStep, textCount - 1);

                        if (!outTextRenderer_.drawText(gameConfig_.introText_[textIndex], 75.0f, 75.0f))
                        {
                            return 1;
                        }
                    }

                    if (!outRenderer_.present())
                    {
                        return 1;
                    }
                }
                break;
                case GameFlowState::Gameplay:
                    world_.updateTime(outAppTime_.getDeltaTime());

                    const InputSpace::Intention intention = outInputCore_.pollInputRequest();

                    simulationCore_.update(world_, intention, commandBuffer_);

                    commitCommands();

                    if (gamePhase_ != GamePhase::RunGame)
                    {
                        break;
                    }

                    if (!outRenderer_.clear())
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

                    if (!outRenderer_.present())
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

        outGameContentLoader_.releaseLoadData();

        if (!transitionGameFlowState(GameFlowState::Gameplay))
        {
            return {ContentSpace::ResourceError{
                "error: failed to transition audio state.",
                ContentSpace::ResourceErrorType::LoadFailed
            }};
        }

        return {};
    }

    void GameCore::commitCommands() noexcept
    {
        for (const auto& command : commandBuffer_.getCommands())
        {
            std::visit(
                [this](const auto& targetCommand)
                {
                    commitCommand(targetCommand);
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
