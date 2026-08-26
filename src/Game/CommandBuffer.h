#pragma once

#include "RuntimeCommand.h"

#include <span>
#include <vector>

namespace Uncarved::GameSpace
{
    class CommandBuffer final
    {
    public:
        void submit(RuntimeCommand&& command);

        std::span<const RuntimeCommand> getCommands() const noexcept;

        void clearCommands() noexcept;

    private:
        std::vector<RuntimeCommand> commands_{};
    };
} // namespace Uncarved::GameSpace
