#pragma once

namespace Uncarved::GameSpace
{
    class CommandBuffer;
    class World;

    class AiController
    {
    public:
        void updateAi(const World& world, CommandBuffer& commandBuffer);
    };
} // namespace Uncarved::GameSpace
