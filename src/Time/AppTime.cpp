#include "AppTime.h"

namespace Uncarved::TimeSpace
{
    AppTime::AppTime()
    {
        auto now = PhysicalClock::now();

        startTimePoint_ = now;
        currentTimePoint_ = now;
        deltaTime_ = Duration::zero();
        elapsedTime_ = Duration::zero();
        frameIndex_ = 0;
    }

    void AppTime::update()
    {
        auto now = PhysicalClock::now();

        elapsedTime_ = now - startTimePoint_;

        if (!bHasPreviousFrame_)
        {
            deltaTime_ = Duration::zero();
            currentTimePoint_ = now;
            bHasPreviousFrame_ = true;
        }
        else
        {
            deltaTime_ = now - currentTimePoint_;
            currentTimePoint_ = now;
        }

        frameIndex_ += 1;
    }

    TimePoint AppTime::getTimePoint() const noexcept
    {
        return currentTimePoint_;
    }

    Duration AppTime::getDeltaTime() const noexcept
    {
        return deltaTime_;
    }

    Duration AppTime::getElapsedTime() const noexcept
    {
        return elapsedTime_;
    }

    std::uint64_t AppTime::getFrameIndex() const noexcept
    {
        return frameIndex_;
    }
} // namespace Uncarved::TimeSpace
