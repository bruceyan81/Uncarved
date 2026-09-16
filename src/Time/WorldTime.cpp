#include "WorldTime.h"

#include <cmath>

namespace Uncarved::TimeSpace
{
    void WorldTime::updateTime(Duration unscaledDeltaTime) noexcept
    {
        unscaledDeltaTime_ = unscaledDeltaTime;
        unscaledElapsedTime_ += unscaledDeltaTime_;

        deltaTime_ = unscaledDeltaTime * timeDilation_;

        elapsedTime_ += deltaTime_;
    }

    Duration WorldTime::getDeltaTime() const noexcept
    {
        return deltaTime_;
    }

    Duration WorldTime::getUnscaledDeltaTime() const noexcept
    {
        return unscaledDeltaTime_;
    }

    Duration WorldTime::getElapsedTime() const noexcept
    {
        return elapsedTime_;
    }

    Duration WorldTime::getUnscaledElapsedTime() const noexcept
    {
        return unscaledElapsedTime_;
    }

    double WorldTime::getTimeDilation() const noexcept
    {
        return timeDilation_;
    }

    bool WorldTime::trySetTimeDilation(double newTimeDilation) noexcept
    {
        if (!std::isfinite(newTimeDilation) || newTimeDilation <= 0.0)
        {
            return false;
        }

        timeDilation_ = newTimeDilation;
        return true;
    }

} // namespace Uncarved::TimeSpace
