#include "GameState.h"

namespace Uncarved::GameSpace
{
    void GameState::initialize(int health, int score) noexcept
    {
        health_ = health;
        score_ = score;
    }

    void GameState::applyDamage(int amount) noexcept
    {
        health_ -= amount;
    }

    void GameState::addScore(int amount) noexcept
    {
        score_ += amount;
    }
} // namespace Uncarved::GameSpace
