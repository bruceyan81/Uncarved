#include "Input.h"

#include "Command.h"

#include <SDL3/SDL.h>

namespace Uncarved::InputSpace
{
    Intention InputCore::pollInputRequest() const
    {
        SDL_Event event{};

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                return Intention::Quit;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
            {
                if (event.key.scancode == SDL_SCANCODE_RETURN || event.key.scancode == SDL_SCANCODE_SPACE)
                {
                    return Intention::NextImage;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                return Intention::NextImage;
            }
        }

        return Intention::None;
    }
} // namespace Uncarved::InputSpace
