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

            if (event.type_ == EventType::KeyDown && !event.keyRepeat_)
            {
                if (event.keyScancode_.has_value()
                    && (*(event.keyScancode_) == KeyboardScancode::Return
                        || *(event.keyScancode_) == KeyboardScancode::Space))
                {
                    return Intention::AdvanceIntro;
                }
            }

            if (event.type_ == EventType::MouseButtonDown && event.mouseButton_.has_value()
                && event.mouseButton_ == MouseButton::Left)
            {
                return Intention::AdvanceIntro;
            }
        }

        return Intention::None;
    }
} // namespace Uncarved::InputSpace
