#include "EventPump.h"

#include "PlatformEvent.h"

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

        void translatePlatformEvent(PlatformEvent& platformEvent, const SDL_Event& sdlEvent)
        {
            platformEvent = {};

            switch (sdlEvent.type)
            {
                case SDL_EventType::SDL_EVENT_QUIT:
                {
                    platformEvent.type_ = PlatformEventType::Quit;
                }
                break;

                case SDL_EventType::SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    platformEvent.type_ = PlatformEventType::WindowCloseRequested;
                }
                break;

                case SDL_EventType::SDL_EVENT_KEY_DOWN:
                {
                    platformEvent.keyScancode_ = translateScancode(sdlEvent.key.scancode);
                    platformEvent.type_ = sdlEvent.key.repeat ? PlatformEventType::KeyRepeat : PlatformEventType::KeyPressed;
                }
                break;

                case SDL_EventType::SDL_EVENT_KEY_UP:
                {
                    platformEvent.keyScancode_ = translateScancode(sdlEvent.key.scancode);
                    platformEvent.type_ = PlatformEventType::KeyReleased;
                }
                break;

                case SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    platformEvent.type_ = PlatformEventType::MouseButtonDown;
                    platformEvent.mouseButton_ = translateMouseButton(sdlEvent.button);
                }
                break;

                default:
                {
                    platformEvent.type_ = PlatformEventType::None;
                }
                break;
            }
        }

        bool pollEvent(PlatformEvent& platformEvent)
        {
            SDL_Event sdlEvent{};

            if (SDL_PollEvent(&sdlEvent))
            {
                translatePlatformEvent(platformEvent, sdlEvent);
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

    bool EventPump::pollEvent(PlatformEvent& platformEvent)
    {
        return impl_->pollEvent(platformEvent);
    }
} // namespace Uncarved::PlatformSpace
