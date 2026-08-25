#pragma once

namespace Uncarved::GameSpace
{
    class GameState final
    {
    public:
        void initialize(int health, int score) noexcept;

        void applyDamage(int amount) noexcept;

        void addScore(int amount) noexcept;

        int getHealth() const noexcept
        {
            return health_;
        }

        int getScore() const noexcept
        {
            return score_;
        }

    private:
        int health_{};
        int score_{};
    };
} // namespace Uncarved::GameSpace
