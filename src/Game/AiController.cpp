#include "AiController.h"

#include "GameState.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Uncarved::GameSpace
{
    void AiController::updateAi(GameState& gameState)
    {
        const ObjectSpace::Actor* playerPtr = gameState.getPlayer();

        const auto& occupancyGrid = gameState.getNpcOccupancyGrid();
        const auto& blockingOccupancyGrid = gameState.getBlockingOccupancyGrid();

        for (auto& actor : gameState.getActors())
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
                gameState.updateNpcOccupancyGrid(position.y * kMapWidth + position.x, -1);

                if (bIsBlocking)
                {
                    gameState.updateBlockingOccupancyGrid(position.y * kMapWidth + position.x, -1);
                }

                actor.moveBy(velocity);

                gameState.updateNpcOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);

                if (bIsBlocking)
                {
                    gameState.updateBlockingOccupancyGrid(actor.getPosition().y * kMapWidth + actor.getPosition().x, 1);
                }
            }
            else
            {
                actor.updateVelocity(-velocity);
            }
        }
    }
} // namespace Uncarved::GameSpace
