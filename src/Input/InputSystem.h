#pragma once

#include "Key.h"

#include <array>
#include <cstddef>

namespace Uncarved
{
    class PlatformEvent;
} // namespace Uncarved

namespace Uncarved::InputSpace
{
    class InputSystem final
    {
    public:
        InputSystem() = default;

        InputSystem(const InputSystem&) = delete;
        InputSystem& operator=(const InputSystem&) = delete;

        InputSystem(InputSystem&& other) noexcept = delete;
        InputSystem& operator=(InputSystem&& other) noexcept = delete;

        ~InputSystem();

        void beginFrame();

        void processPlatformEvent(const PlatformEvent& platformEvent);

        bool isKeyDown(Key key) const;
        bool wasKeyJustPressed(Key key) const;
        bool wasKeyJustReleased(Key key) const;

    private:
        std::array<KeyState, static_cast<std::size_t>(Key::Count)> keysState_{};
    };
} // namespace Uncarved::InputSpace
