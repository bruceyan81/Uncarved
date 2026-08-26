#include "AiController.h"

#include "CommandBuffer.h"
#include "World.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Uncarved::GameSpace
{
    void AiController::updateAi(const World& world, CommandBuffer& commandBuffer)
    {
        for (const auto& actor : world.getActors())
        {
            const glm::ivec2 velocity = actor.getVelocity();

            if (velocity.x == 0 && velocity.y == 0)
            {
                continue;
            }

            commandBuffer.submit(MoveActorCommand{actor.getId(), velocity});
        }
    }
} // namespace Uncarved::GameSpace
