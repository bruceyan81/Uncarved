#pragma once

#include "AiController.h"

#include <glm/glm.hpp>

namespace Uncarved::InputSpace
{
    enum class Intention;
}

namespace Uncarved::GameSpace
{
    class GameState;

    enum class GameTickOutcome
    {
        None,
        Continue,
        QuitRequested,
        Won,
        Lost,
        Count
    };

    struct SimulationResult final
    {
        GameTickOutcome outcome_;
    };

    class SimulationCore
    {
    public:
        SimulationResult update(GameState& gameState, const InputSpace::Intention intention);

    private:
        AiController aiController_{};

        bool canPlayerMoveTo(const GameState& gameState, const glm::ivec2& nextPosition) const noexcept;
    };
} // namespace Uncarved::GameSpace
