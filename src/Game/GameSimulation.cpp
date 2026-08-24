#include "GameSimulation.h"

#include "World.h"

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
    SimulationResult SimulationCore::update(World& world, const InputSpace::Intention intention)
    {
        this->aiController_.updateAi(world);

        switch (intention)
        {
            case InputSpace::Intention::ToNorth:
            {
                if (this->canPlayerMoveTo(world, kToNorth))
                {
                    world.movePlayerBy(kToNorth);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToEast:
            {
                if (this->canPlayerMoveTo(world, kToEast))
                {
                    world.movePlayerBy(kToEast);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToSouth:
            {
                if (this->canPlayerMoveTo(world, kToSouth))
                {
                    world.movePlayerBy(kToSouth);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::ToWest:
            {
                if (this->canPlayerMoveTo(world, kToWest))
                {
                    world.movePlayerBy(kToWest);
                }
            }
                return {GameTickOutcome::Continue};
            case InputSpace::Intention::Quit:
                return {GameTickOutcome::QuitRequested};
            default:
                return {GameTickOutcome::None};
        }
    }

    bool SimulationCore::canPlayerMoveTo(const World& world, const glm::ivec2& delta) const noexcept
    {
        const ObjectSpace::Actor* playerPtr = world.getPlayer();

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

        if (world.getBlockingOccupancyGrid()[nextPosition.y * kMapWidth + nextPosition.x] > 0)
        {
            return false;
        }

        return true;
    }
} // namespace Uncarved::GameSpace
