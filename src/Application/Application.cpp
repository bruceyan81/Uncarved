#include "Application.h"

#include "Game/Game.h"
#include "Game/Interaction.h"
#include "Input/Input.h"
#include "View/Camera.h"
#include "View/Renderer.h"

namespace Uncarved::ApplicationSpace
{
    ApplicationCore::ApplicationCore()
    {
        applicationState_ = ApplicationState::Init;
    }

    int ApplicationCore::initializeApplication()
    {
        this->gameContentLoader_.checkResourceDirectory();
        this->gameContentLoader_.checkGameConfig();

        if (!this->gameContentLoader_.loadGameConfig())
        {
            return 1;
        }

        if (this->gameContentLoader_.checkRenderingConfig())
        {
            this->gameContentLoader_.loadRenderingConfig();
        }

        return 0;
    }

    int ApplicationCore::launch()
    {
        if (this->initializeApplication() == 0)
        {
            this->applicationState_ = ApplicationState::Running;

            const auto& gameConfig = this->gameContentLoader_.getGameConfig();
            const auto& renderingConfig = this->gameContentLoader_.getRenderingConfig();

            const auto viewportWidthIt = renderingConfig.find("x_resolution");
            const auto viewportHeightIt = renderingConfig.find("y_resolution");

            const int viewportWidth =
                viewportWidthIt != renderingConfig.end() ? std::get<int>(viewportWidthIt->second) : 0;
            const int viewportHeight =
                viewportHeightIt != renderingConfig.end() ? std::get<int>(viewportHeightIt->second) : 0;

            GameSpace::GameCore gameCore{
                GameSpace::SimulationCore{},
                GameSpace::InteractionCore{},
                InputSpace::InputCore{},
                ViewSpace::Renderer{},
                ViewSpace::CameraManager{viewportWidth, viewportHeight, std::size_t{13}, std::size_t{9}},
                {
                    gameConfig.find("health") != gameConfig.end() ? std::get<int>(gameConfig.find("health")->second) : 3,
                    gameConfig.find("score") != gameConfig.end() ? std::get<int>(gameConfig.find("score")->second) : 0,
                    gameConfig.find("game_start_message") != gameConfig.end()
                        ? std::get<std::string>(gameConfig.find("game_start_message")->second)
                        : "",
                    gameConfig.find("game_over_bad_message") != gameConfig.end()
                        ? std::get<std::string>(gameConfig.find("game_over_bad_message")->second)
                        : "",
                    gameConfig.find("game_over_good_message") != gameConfig.end()
                        ? std::get<std::string>(gameConfig.find("game_over_good_message")->second)
                        : ""
                },
                this->gameContentLoader_
            };

            if (gameCore.launch() == 0)
            {
                this->applicationState_ = ApplicationState::End;
                return 0;
            }
        }
        return 1;
    }
} // namespace Uncarved::ApplicationSpace
