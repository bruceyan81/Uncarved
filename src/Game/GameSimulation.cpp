#include "GameSimulation.h"

#include "CommandBuffer.h"
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
    void SimulationCore::update(const World& world, const InputSpace::Intention intention, CommandBuffer& commandBuffer)
    {
        this->aiController_.updateAi(world, commandBuffer);

        switch (intention)
        {
            case InputSpace::Intention::ToNorth:
                submitPlayerMovement(world, kToNorth, commandBuffer);
                break;
            case InputSpace::Intention::ToEast:
                submitPlayerMovement(world, kToEast, commandBuffer);
                break;
            case InputSpace::Intention::ToSouth:
                submitPlayerMovement(world, kToSouth, commandBuffer);
                break;
            case InputSpace::Intention::ToWest:
                submitPlayerMovement(world, kToWest, commandBuffer);
                break;
            case InputSpace::Intention::Quit:
                commandBuffer.submit(QuitCommand{});
                break;
            default:
                break;
        }
    }

    void SimulationCore::submitPlayerMovement(const World& world, const glm::ivec2& delta, CommandBuffer& commandBuffer)
    {
        const ObjectSpace::Actor* playerPtr = world.getPlayer();

        if (playerPtr == nullptr)
        {
            return;
        }

        commandBuffer.submit(MoveActorCommand{playerPtr->getId(), delta});
    }
} // namespace Uncarved::GameSpace
