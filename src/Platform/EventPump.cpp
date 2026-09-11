#include "EventPump.h"

#include "Event.h"

#include <SDL3/SDL.h>

#include <memory>
#include <optional>

namespace Uncarved::PlatformSpace
{
    struct EventPump::Impl final
    {
        Impl() = default;

        ~Impl() = default;

        std::optional<MouseButton> translateMouseButton(const SDL_MouseButtonEvent& mouseButtonEvent)
        {
            switch (mouseButtonEvent.button)
            {
                case 1:
                    return MouseButton::Left;
                case 2:
                    return MouseButton::Middle;
                case 3:
                    return MouseButton::Right;
                default:
                    return std::nullopt;
            }
        }

        std::optional<KeyboardScancode> translateScancode(const SDL_Scancode& scancode)
        {
            switch (scancode)
            {
                case SDL_SCANCODE_RETURN:
                    return KeyboardScancode::Return;
                case SDL_SCANCODE_SPACE:
                    return KeyboardScancode::Space;
                default:
                    return std::nullopt;
            }
        }

        void translateEvent(Event& event, const SDL_Event& sdlEvent)
        {
            event = {};

            switch (sdlEvent.type)
            {
                case SDL_EventType::SDL_EVENT_QUIT:
                    event.type_ = EventType::Quit;
                    break;
                case SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    event.type_ = EventType::WindowCloseRequested;
                    break;
                case SDL_EventType::SDL_EVENT_KEY_DOWN:
                    event.type_ = EventType::KeyDown;
                    event.keyScancode_ = translateScancode(sdlEvent.key.scancode);
                    event.keyRepeat_ = sdlEvent.key.repeat;
                    break;
                case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
                    event.type_ = EventType::MouseButtonDown;
                    event.mouseButton_ = translateMouseButton(sdlEvent.button);
                    break;
                default:
                    event.type_ = EventType::None;
                    break;
            }
        }

        bool pollEvent(Event& event)
        {
            SDL_Event sdlEvent{};

            if (SDL_PollEvent(&sdlEvent))
            {
                translateEvent(event, sdlEvent);
            }
            else
            {
                return false;
            }

            return true;
        }
    };

    EventPump::EventPump() : impl_(std::make_unique<Impl>())
    {
    }

    EventPump::~EventPump() = default;

    bool EventPump::pollEvent(Event& event)
    {
        return impl_->pollEvent(event);
    }
} // namespace Uncarved::PlatformSpace
