#pragma once

#include <string>
#include <vector>

namespace Uncarved::GameSpace
{
    struct GameConfig final
    {
        int health_{};
        int score_{};

        std::vector<std::string> introText_{};
    };
} // namespace Uncarved::GameSpace
