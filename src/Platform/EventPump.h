#pragma once

#include <memory>

namespace Uncarved
{
    class Event;
}

namespace Uncarved::PlatformSpace
{
    class EventPump final
    {
    public:
        EventPump();

        ~EventPump();

        bool pollEvent(Event& event);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
