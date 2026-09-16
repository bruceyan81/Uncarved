#pragma once

#include "TimeTypes.h"

#include <cstdint>

namespace Uncarved::TimeSpace
{
    class AppTime final
    {
    public:
        AppTime();

        void update();

        TimePoint     getTimePoint() const noexcept;
        Duration      getDeltaTime() const noexcept;
        Duration      getElapsedTime() const noexcept;
        std::uint64_t getFrameIndex() const noexcept;

    private:
        TimePoint     startTimePoint_{};
        TimePoint     currentTimePoint_{};
        Duration      deltaTime_{};
        Duration      elapsedTime_{};
        std::uint64_t frameIndex_{};
        bool          bHasPreviousFrame_{false};
    };
} // namespace Uncarved::TimeSpace
