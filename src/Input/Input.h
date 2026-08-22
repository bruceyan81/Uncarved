#pragma once

namespace Uncarved::InputSpace
{
    enum class Intention;

    class InputCore
    {
    public:
        Intention pollInputRequest() const;
    };
} // namespace Uncarved::InputSpace
