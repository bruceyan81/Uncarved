#include "InputSystem.h"

#include "Platform/PlatformEvent.h"

#include <optional>

namespace Uncarved::InputSpace
{
    namespace
    {
        bool isLegalKey(Uncarved::InputSpace::Key key)
        {
            if (key == Uncarved::InputSpace::Key::None || key == Uncarved::InputSpace::Key::Count)
            {
                return false;
            }

            return true;
        }

        std::optional<Uncarved::InputSpace::Key> mapScancode(std::optional<Uncarved::KeyboardScancode> scancode)
        {
            if (!scancode.has_value())
            {
                return std::nullopt;
            }

            switch (*scancode)
            {
                case Uncarved::KeyboardScancode::W:
                    return Uncarved::InputSpace::Key::W;
                case Uncarved::KeyboardScancode::A:
                    return Uncarved::InputSpace::Key::A;
                case Uncarved::KeyboardScancode::S:
                    return Uncarved::InputSpace::Key::S;
                case Uncarved::KeyboardScancode::D:
                    return Uncarved::InputSpace::Key::D;
                case Uncarved::KeyboardScancode::ArrowUp:
                    return Uncarved::InputSpace::Key::ArrowUp;
                case Uncarved::KeyboardScancode::ArrowDown:
                    return Uncarved::InputSpace::Key::ArrowDown;
                case Uncarved::KeyboardScancode::ArrowLeft:
                    return Uncarved::InputSpace::Key::ArrowLeft;
                case Uncarved::KeyboardScancode::ArrowRight:
                    return Uncarved::InputSpace::Key::ArrowRight;
                case Uncarved::KeyboardScancode::Space:
                    return Uncarved::InputSpace::Key::Space;
                case Uncarved::KeyboardScancode::Return:
                    return Uncarved::InputSpace::Key::Enter;
                default:
                    return std::nullopt;
            }
        }
    } // namespace

    InputSystem::~InputSystem() = default;

    void InputSystem::beginFrame()
    {
        for (auto& state : keysState_)
        {
            state.bPressed_ = false;
            state.bReleased_ = false;
        }
    }

    void InputSystem::processPlatformEvent(const PlatformEvent& platformEvent)
    {
        auto key = mapScancode(platformEvent.keyScancode_);

        if (key.has_value())
        {
            switch (platformEvent.type_)
            {
                case PlatformEventType::KeyPressed:
                    if (!keysState_[static_cast<std::size_t>(*key)].bDown_)
                    {
                        keysState_[static_cast<std::size_t>(*key)].bPressed_ = true;
                        keysState_[static_cast<std::size_t>(*key)].bDown_ = true;
                    }
                    break;
                case PlatformEventType::KeyReleased:
                    if (keysState_[static_cast<std::size_t>(*key)].bDown_)
                    {
                        keysState_[static_cast<std::size_t>(*key)].bDown_ = false;
                        keysState_[static_cast<std::size_t>(*key)].bReleased_ = true;
                    }
                    break;
                case PlatformEventType::KeyRepeat:
                    break;
                default:
                    break;
            }
        }
    }

    bool InputSystem::isKeyDown(Key key) const
    {
        if (!isLegalKey(key))
        {
            return false;
        }

        return keysState_[static_cast<std::size_t>(key)].bDown_;
    }

    bool InputSystem::wasKeyJustPressed(Key key) const
    {
        if (!isLegalKey(key))
        {
            return false;
        }

        return keysState_[static_cast<std::size_t>(key)].bPressed_;
    }

    bool InputSystem::wasKeyJustReleased(Key key) const
    {
        if (!isLegalKey(key))
        {
            return false;
        }

        return keysState_[static_cast<std::size_t>(key)].bReleased_;
    }
} // namespace Uncarved::InputSpace
