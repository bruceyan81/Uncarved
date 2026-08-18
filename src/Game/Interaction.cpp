#include "Interaction.h"

#include "Gameplay.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include <utility>

namespace Uncarved
{
    namespace
    {
        bool isNearby(const glm::ivec2& npcPosition, const glm::ivec2& playerPosition) noexcept
        {
            constexpr std::array<glm::ivec2, 8> kNearbyCoordinates{
                glm::ivec2{-1, -1},
                glm::ivec2{0, -1},
                glm::ivec2{1, -1},
                glm::ivec2{-1, 0},
                glm::ivec2{1, 0},
                glm::ivec2{-1, 1},
                glm::ivec2{0, 1},
                glm::ivec2{1, 1},
            };

            for (const auto& coord : kNearbyCoordinates)
            {
                glm::ivec2 tempNpcPosition = npcPosition;

                tempNpcPosition += coord;

                if (tempNpcPosition == playerPosition)
                {
                    return true;
                }
            }
            return false;
        }

        GameSpace::DialogueCommand
        filterDialogue(std::string_view dialogue, std::optional<std::string>& outSceneName) noexcept
        {
            GameSpace::DialogueCommand resultCommand = GameSpace::DialogueCommand::None;

            if (dialogue.empty())
            {
                return resultCommand;
            }

            constexpr std::array<
                std::pair<std::string_view, GameSpace::DialogueCommand>,
                static_cast<std::size_t>(GameSpace::DialogueCommand::Count) - 1>
                kMapping{
                    std::pair{std::string_view{"[score up]"}, GameSpace::DialogueCommand::ScoreUp},
                    std::pair{std::string_view{"[health down]"}, GameSpace::DialogueCommand::HealthDown},
                    std::pair{std::string_view{"[proceed to]"}, GameSpace::DialogueCommand::SceneTransition},
                    std::pair{std::string_view{"[you win]"}, GameSpace::DialogueCommand::PlayerWin},
                    std::pair{std::string_view{"[game over]"}, GameSpace::DialogueCommand::GameOver},
                };

            for (auto& [targetSubstring, intention] : kMapping)
            {
                if (dialogue.find(targetSubstring) != std::string_view::npos)
                {
                    resultCommand = intention;

                    if (resultCommand == GameSpace::DialogueCommand::SceneTransition)
                    {
                        std::size_t begin = dialogue.rfind("[");

                        outSceneName = dialogue.substr(begin + 1, dialogue.size() - begin - 2);
                    }

                    return resultCommand;
                }
            }

            return resultCommand;
        }
    } // namespace
} // namespace Uncarved

namespace Uncarved::GameSpace
{
    void InteractionCore::dialogueInteraction(const GameState& gameState)
    {
        const auto& playerPtr = gameState.getPlayer();

        if (playerPtr == nullptr)
        {
            return;
        }

        results_.clear();

        std::optional<std::string> sceneName = std::nullopt;
        DialogueCommand            command = DialogueCommand::None;

        const glm::ivec2 playerPosition = playerPtr->getPosition();

        for (const auto& actor : gameState.getActorsReadOnly())
        {
            if (actor == *playerPtr)
            {
                continue;
            }

            const glm::ivec2 actorPosition = actor.getPosition();

            if (actorPosition == playerPosition)
            {
                command = filterDialogue(actor.getContactDialogue(), sceneName);
                results_.emplace_back(
                    InteractionResult{actor.getId(), actor.getContactDialogue(), command, std::move(sceneName)}
                );
                continue;
            }

            if (isNearby(actorPosition, playerPosition))
            {
                command = filterDialogue(actor.getNearbyDialogue(), sceneName);
                results_.emplace_back(
                    InteractionResult{actor.getId(), actor.getNearbyDialogue(), command, std::move(sceneName)}
                );
            }
        }

        std::sort(
            results_.begin(),
            results_.end(),
            [](const InteractionResult& first, const InteractionResult& second) {
                return first.sourceActorId_ < second.sourceActorId_;
            }
        );
    }

    const std::vector<InteractionResult>& InteractionCore::getInteractionResults() const noexcept
    {
        return results_;
    }

    void InteractionCore::resolveInteractionResult(GameState& gameState) const noexcept
    {
        for (const auto& result : results_)
        {
            auto* actor = gameState.getActorById(result.sourceActorId_);

            if (actor == nullptr)
            {
                continue;
            }

            std::string nextSceneName;

            if (result.sceneName_)
            {
                nextSceneName = result.sceneName_.value();
            }

            switch (result.intention_)
            {
                case DialogueCommand::None:
                    break;
                case DialogueCommand::ScoreUp:
                    if (!actor->getHasUppedScore())
                    {
                        gameState.updateScore(gameState.getScore() + 1);
                        actor->updateHasUppedScore(true);
                    }
                    break;
                case DialogueCommand::HealthDown:
                    gameState.updateHealth(gameState.getHealth() - 1);

                    if (gameState.getHealth() <= 0)
                    {
                        gameState.createRequest(DialogueCommand::GameOver, nextSceneName);
                    }
                    break;
                case DialogueCommand::SceneTransition:
                    gameState.createRequest(DialogueCommand::SceneTransition, nextSceneName);
                    break;
                case DialogueCommand::PlayerWin:
                    gameState.createRequest(DialogueCommand::PlayerWin, nextSceneName);
                    break;
                case DialogueCommand::GameOver:
                    gameState.createRequest(DialogueCommand::GameOver, nextSceneName);
                    break;
                default:
                    break;
            }
        }
    }
} // namespace Uncarved::GameSpace
