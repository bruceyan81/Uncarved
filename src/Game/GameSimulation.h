#pragma once

#include "AiController.h"

#include <glm/glm.hpp>

namespace Uncarved::InputSpace
{
    enum class Intention;
}

namespace Uncarved::GameSpace
{
    class GameStateManager;

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
        SimulationResult update(GameStateManager& gameStateManager, const InputSpace::Intention intention);

    private:
        AiController aiController_{};

        bool canPlayerMoveTo(const GameStateManager& gameStateManager, const glm::ivec2& nextPosition) const noexcept;
    };
} // namespace Uncarved::GameSpace
