#pragma once

#include <chrono>

namespace Uncarved::TimeSpace
{
    using PhysicalClock = std::chrono::steady_clock;
    using TimePoint = PhysicalClock::time_point;
    using Duration = std::chrono::duration<double>;
} // namespace Uncarved::TimeSpace
