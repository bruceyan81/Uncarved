#pragma once

namespace Uncarved::InputSpace
{
    enum class Intention;

    class InputCore
    {
    public:
        bool pollQuitRequest() const;
    };
} // namespace Uncarved::InputSpace
