#pragma once

namespace Uncarved::InputSpace
{
    enum class Intention;

    class InputCore
    {
    public:
        Intention transitionRawCommand() const;
    };
} // namespace Uncarved::InputSpace
