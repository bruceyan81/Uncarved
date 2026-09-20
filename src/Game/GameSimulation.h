#pragma once

#include "AiController.h"

#include <glm/glm.hpp>

namespace Uncarved::InputSpace
{
    enum class Intention;
}

namespace Uncarved::GameSpace
{
    class CommandBuffer;
    class World;

    class SimulationCore final
    {
    public:
        void update(const World& world, const InputSpace::Intention intention, CommandBuffer& commandBuffer);

    private:
        AiController aiController_{};

        void submitPlayerMovement(const World& world, const glm::ivec2& delta, CommandBuffer& commandBuffer);
    };
} // namespace Uncarved::GameSpace
