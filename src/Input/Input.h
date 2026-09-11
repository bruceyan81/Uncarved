#pragma once

namespace Uncarved::PlatformSpace
{
    class EventPump;
}

namespace Uncarved::InputSpace
{
    enum class Intention;

    class InputCore
    {
    public:
        explicit InputCore(PlatformSpace::EventPump& outEventPump);

        InputCore(const InputCore&) = delete;
        InputCore& operator=(const InputCore&) = delete;

        InputCore(InputCore&& other) noexcept = delete;
        InputCore& operator=(InputCore&&) noexcept = delete;

        ~InputCore();

        Intention pollInputRequest() const;

    private:
        PlatformSpace::EventPump& outEventPump_;
    };
} // namespace Uncarved::InputSpace
