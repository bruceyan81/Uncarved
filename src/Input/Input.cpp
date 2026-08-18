#include "Input.h"

#include "Command.h"

#include <iostream>
#include <string>

namespace Uncarved::InputSpace
{
    Intention InputCore::transitionRawCommand() const
    {
        std::string rawCommand;

        if (!(std::cin >> rawCommand))
        {
            return Intention::Quit;
        }

        if (rawCommand == "N" || rawCommand == "n")
        {
            return Intention::ToNorth;
        }
        else if (rawCommand == "E" || rawCommand == "e")
        {
            return Intention::ToEast;
        }
        else if (rawCommand == "S" || rawCommand == "s")
        {
            return Intention::ToSouth;
        }
        else if (rawCommand == "W" || rawCommand == "w")
        {
            return Intention::ToWest;
        }
        else if (rawCommand == "quit")
        {
            return Intention::Quit;
        }
        else
        {
            return Intention::None;
        }
    }
} // namespace Uncarved::InputSpace
