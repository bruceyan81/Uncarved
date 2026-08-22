#include "AiController.h"

#include "GameStateManager.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Uncarved::GameSpace
{
    void AiController::updateAi(GameStateManager& gameStateManager)
    {
        const ObjectSpace::Actor* playerPtr = gameStateManager.getPlayer();

        const auto& occupancyGrid = gameStateManager.getNpcOccupancyGrid();
        const auto& blockingOccupancyGrid = gameStateManager.getBlockingOccupancyGrid();

        for (auto& actor : gameStateManager.getActors())
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
                gameStateManager.updateNpcOccupancyGrid(position.y * kMapWidth + position.x, -1);

                if (bIsBlocking)
                {
                    gameStateManager.updateBlockingOccupancyGrid(position.y * kMapWidth + position.x, -1);
                }

                actor.moveBy(velocity);

                gameStateManager.updateNpcOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);

                if (bIsBlocking)
                {
                    gameStateManager.updateBlockingOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);
                }
            }
            else
            {
                actor.updateVelocity(-velocity);
            }
        }
    }
} // namespace Uncarved::GameSpace
