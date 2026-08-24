#include "AiController.h"

#include "World.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Uncarved::GameSpace
{
    void AiController::updateAi(World& world)
    {
        const ObjectSpace::Actor* playerPtr = world.getPlayer();

        const auto& occupancyGrid = world.getNpcOccupancyGrid();
        const auto& blockingOccupancyGrid = world.getBlockingOccupancyGrid();

        for (auto& actor : world.getActors())
        {
            if (playerPtr != nullptr && actor == *playerPtr)
            {
                continue;
            }

            const glm::ivec2 velocity = actor.getVelocity();

            if (velocity.x == 0 && velocity.y == 0)
            {
                continue;
            }

            const glm::ivec2 position = actor.getPosition();

            const glm::ivec2 tryMovementPosition = position + velocity;

            if (tryMovementPosition.x < 0
                || tryMovementPosition.y < 0
                || tryMovementPosition.x >= kMapWidth
                || tryMovementPosition.y >= kMapHeight)
            {
                continue;
            }

            if (blockingOccupancyGrid[tryMovementPosition.y * kMapWidth + tryMovementPosition.x] > 0)
            {
                actor.updateVelocity(-velocity);
                continue;
            }

            const bool bIsBlocking = actor.getBlocking();

            if (bIsBlocking && (playerPtr != nullptr) && (tryMovementPosition == playerPtr->getPosition()))
            {
                actor.updateVelocity(-velocity);
                continue;
            }

            bool bIsExistOtherNpc = occupancyGrid[tryMovementPosition.y * kMapWidth + tryMovementPosition.x] > 0;

            if (!bIsExistOtherNpc)
            {
                world.updateNpcOccupancyGrid(position.y * kMapWidth + position.x, -1);

                if (bIsBlocking)
                {
                    world.updateBlockingOccupancyGrid(position.y * kMapWidth + position.x, -1);
                }

                actor.moveBy(velocity);

                world.updateNpcOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);

                if (bIsBlocking)
                {
                    world.updateBlockingOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);
                }
            }
            else
            {
                actor.updateVelocity(-velocity);
            }
        }
    }
} // namespace Uncarved::GameSpace
