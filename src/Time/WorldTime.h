#pragma once

#include "TimeTypes.h"

namespace Uncarved::TimeSpace
{
    class WorldTime final
    {
    public:
        WorldTime() = default;

        void updateTime(Duration unscaledDeltaTime) noexcept;

        Duration getDeltaTime() const noexcept;
        Duration getUnscaledDeltaTime() const noexcept;

        Duration getElapsedTime() const noexcept;
        Duration getUnscaledElapsedTime() const noexcept;

        double   getTimeDilation() const noexcept;

        bool trySetTimeDilation(double newTimeDilation) noexcept;

    private:
        Duration deltaTime_{};
        Duration unscaledDeltaTime_{};
        Duration elapsedTime_{};
        Duration unscaledElapsedTime_{};
        double   timeDilation_{1.0};
    };
} // namespace Uncarved::TimeSpace
