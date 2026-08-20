#include "Input.h"

#include "Command.h"

#include <SDL3/SDL.h>

namespace Uncarved::InputSpace
{
    bool InputCore::pollQuitRequest() const
    {
        SDL_Event event{};

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                return true;
            }
        }

        return false;
    }
} // namespace Uncarved::InputSpace
