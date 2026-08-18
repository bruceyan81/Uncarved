#include "GameSimulation.h"

#include "GameState.h"

#include "Input/Command.h"

namespace Uncarved
{
    namespace
    {
        constexpr glm::ivec2 kToNorth{0, -1};
        constexpr glm::ivec2 kToEast{1, 0};
        constexpr glm::ivec2 kToSouth{0, 1};
        constexpr glm::ivec2 kToWest{-1, 0};
    } // namespace
} // namespace Uncarved

namespace Uncarved::GameSpace
{
    SimulationResult SimulationCore::update(GameState& gameState, const InputSpace::Intention intention)
    {
        this->aiController_.updateAi(gameState);

        switch (intention)
        {
            case InputSpace::Intention::ToNorth:
            {
                if (this->canPlayerMoveTo(gameState, kToNorth))
                {
                    gameState.movePlayerBy(kToNorth);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToEast:
            {
                if (this->canPlayerMoveTo(gameState, kToEast))
                {
                    gameState.movePlayerBy(kToEast);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToSouth:
            {
                if (this->canPlayerMoveTo(gameState, kToSouth))
                {
                    gameState.movePlayerBy(kToSouth);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToWest:
            {
                if (this->canPlayerMoveTo(gameState, kToWest))
                {
                    gameState.movePlayerBy(kToWest);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::Quit:
                return {GameTickOutcome::QuitRequested};
            default:
                return {GameTickOutcome::None};
        }
    }

    bool SimulationCore::canPlayerMoveTo(const GameState& gameState, const glm::ivec2& delta) const noexcept
    {
        const ObjectSpace::Actor* playerPtr = gameState.getPlayer();

        if (playerPtr == nullptr)
        {
            return false;
        }

        const glm::ivec2 playerPosition = playerPtr->getPosition();
        const glm::ivec2 nextPosition = playerPosition + delta;

        if (nextPosition.x < 0 || nextPosition.x >= kMapWidth || nextPosition.y < 0 || nextPosition.y >= kMapHeight)
        {
            return false;
        }

        if (gameState.getBlockingOccupancyGrid()[nextPosition.y * kMapWidth + nextPosition.x] > 0)
        {
            return false;
        }

        return true;
    }
} // namespace Uncarved::GameSpace
