#pragma once

#include <string>

namespace Uncarved::GameSpace
{
    struct GameConfig final
    {
        int health_{};
        int score_{};

        std::string gameStartMessage_;
        std::string gameOverBadMessage_;
        std::string gameOverGoodMessage_;
    };
} // namespace Uncarved::GameSpace
