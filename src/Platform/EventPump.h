#pragma once

#include <memory>

namespace Uncarved
{
    class PlatformEvent;
}

namespace Uncarved::PlatformSpace
{
    class EventPump final
    {
    public:
        EventPump();

        ~EventPump();

        bool pollEvent(PlatformEvent& platformEvent);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
