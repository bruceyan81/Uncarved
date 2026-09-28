#include "Input.h"

#include "Command.h"

#include "Platform/Event.h"
#include "Platform/EventPump.h"

namespace Uncarved::InputSpace
{
    InputCore::InputCore(PlatformSpace::EventPump& outEventPump) : outEventPump_(outEventPump)
    {
    }

    InputCore::~InputCore() = default;

    Intention InputCore::pollInputRequest() const
    {
        Event event{};

        while (outEventPump_.pollEvent(event))
        {
            if (event.type_ == EventType::Quit || event.type_ == EventType::WindowCloseRequested)
            {
                return Intention::Quit;
            }
        }

        return Intention::None;
    }
} // namespace Uncarved::InputSpace
