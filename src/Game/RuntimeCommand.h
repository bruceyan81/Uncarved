#pragma once

#include <variant>

namespace Uncarved::GameSpace
{
    struct ExitRuntimeCommand final
    {
        // placeholder
    };

    using RuntimeCommand = std::variant<ExitRuntimeCommand>;
} // namespace Uncarved::GameSpace
