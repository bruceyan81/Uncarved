#pragma once

#include "Object/Actor.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Uncarved::GameSpace
{
    struct GameState;

    class GameStateManager;

    enum class DialogueCommand;

    struct InteractionResult final
    {
        ObjectSpace::ActorId       sourceActorId_;
        std::string_view           dialogue_;
        DialogueCommand            intention_;
        std::optional<std::string> sceneName_{};
    };

    class InteractionCore
    {
    public:
        InteractionCore()
        {
            results_.reserve(9);
        };

        void dialogueInteraction(const GameStateManager& gameState);

        const std::vector<InteractionResult>& getInteractionResults() const noexcept;

        void resolveInteractionResult(GameState& gameState, GameStateManager& gameStateManager) const noexcept;

        void clearResults()
        {
            results_.clear();
        };

    private:
        std::vector<InteractionResult> results_{};
    };
} // namespace Uncarved::GameSpace
