#include "AiController.h"

#include "World.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Uncarved::GameSpace
{
    void AiController::updateAi(World& world)
    {
        const ObjectSpace::Actor* playerPtr = world.getPlayer();

        for (const auto& actor : world.getActors())
        {
            if (playerPtr != nullptr && actor == *playerPtr)
            {
                continue;
            }

            const auto       actorId = actor.getId();
            const glm::ivec2 velocity = actor.getVelocity();

            if (velocity.x == 0 && velocity.y == 0)
            {
                continue;
            }

            const glm::ivec2 nextPosition = actor.getPosition() + velocity;

            if (nextPosition.x < 0 || nextPosition.x >= kMapWidth || nextPosition.y < 0 || nextPosition.y >= kMapHeight)
            {
                if (auto* mutableActor = world.getActorById(actorId))
                {
                    mutableActor->updateVelocity(-velocity);
                }

                continue;
            }

            if (actor.getBlocking() && playerPtr != nullptr && nextPosition == playerPtr->getPosition())
            {
                if (auto* mutableActor = world.getActorById(actorId))
                {
                    mutableActor->updateVelocity(-velocity);
                }

                continue;
            }

            world.moveActorBy(actorId, velocity);
        }
    }
} // namespace Uncarved::GameSpace
