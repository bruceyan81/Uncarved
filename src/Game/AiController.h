#pragma once

namespace Uncarved::GameSpace
{
    class CommandBuffer;
    class World;

    class AiController final
    {
    public:
        void updateAi(const World& world, CommandBuffer& commandBuffer);
    };
} // namespace Uncarved::GameSpace
