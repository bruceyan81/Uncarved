#include "CommandBuffer.h"

#include <utility>

namespace Uncarved::GameSpace
{
    void CommandBuffer::submit(RuntimeCommand&& command)
    {
        commands_.emplace_back(std::move(command));
    }

    std::span<const RuntimeCommand> CommandBuffer::getCommands() const noexcept
    {
        return commands_;
    }

    void CommandBuffer::clearCommands() noexcept
    {
        commands_.clear();
    }
} // namespace Uncarved::GameSpace
