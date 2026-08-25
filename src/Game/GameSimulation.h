#pragma once

#include "AiController.h"

#include <glm/glm.hpp>

namespace Uncarved::InputSpace
{
    enum class Intention;
}

namespace Uncarved::GameSpace
{
    class World;

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
        SimulationResult update(World& world, const InputSpace::Intention intention);

    private:
        AiController aiController_{};

        bool canPlayerMoveTo(const World& world, const glm::ivec2& nextPosition) const noexcept;

        void tryMovePlayer(World& world, const glm::ivec2& delta) noexcept;
    };
} // namespace Uncarved::GameSpace
