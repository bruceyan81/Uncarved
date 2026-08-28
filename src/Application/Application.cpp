#include "Application.h"

#include "WindowManager.h"

#include "Content/FontLoader.h"
#include "Content/ImageLoader.h"
#include "Game/Game.h"
#include "Input/Input.h"
#include "View/Renderer.h"
#include "View/TextRenderer.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Uncarved::ApplicationSpace
{
    ApplicationCore::ApplicationCore()
    {
        applicationState_ = ApplicationState::Init;
    }

    ApplicationCore::~ApplicationCore()
    {
        if (bIsSdlTtfInitialized_)
        {
            TTF_Quit();
        }

        if (bIsSdlInitialized_)
        {
            SDL_Quit();
        }
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

        bIsSdlInitialized_ = SDL_Init(SDL_INIT_VIDEO);

        if (!bIsSdlInitialized_)
        {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
            return 1;
        }

        bIsSdlTtfInitialized_ = TTF_Init();

        if (!bIsSdlTtfInitialized_)
        {
            std::cerr << "TTF_Init failed: " << SDL_GetError() << '\n';
            return 1;
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

            WindowManager windowManager{
                gameConfig.find("game_title") != gameConfig.end()
                    ? std::get<std::string>(gameConfig.find("game_title")->second)
                    : "",
                {
                    (
                        renderingConfig.find("x_resolution") != renderingConfig.end()
                        ? std::get<int>(renderingConfig.find("x_resolution")->second)
                        : 640
                    ),
                    (
                        renderingConfig.find("y_resolution") != renderingConfig.end()
                        ? std::get<int>(renderingConfig.find("y_resolution")->second)
                        : 360
                    )
                }
            };

            if (!windowManager.initializeWindow())
            {
                return 1;
            }

            ViewSpace::Renderer renderer{};

            bool bRendererInitResult = renderer.initializeRenderer(
                windowManager.getWindow(),
                renderingConfig.find("clear_color_r") != renderingConfig.end()
                    ? std::get<int>(renderingConfig.find("clear_color_r")->second)
                    : 0,
                renderingConfig.find("clear_color_g") != renderingConfig.end()
                    ? std::get<int>(renderingConfig.find("clear_color_g")->second)
                    : 0,
                renderingConfig.find("clear_color_b") != renderingConfig.end()
                    ? std::get<int>(renderingConfig.find("clear_color_b")->second)
                    : 0
            );

            if (!bRendererInitResult)
            {
                return 1;
            }

            const auto& introImages =
                std::get<std::vector<std::string>>(gameConfig.at("intro_image"));
            const auto& introTexts =
                std::get<std::vector<std::string>>(gameConfig.at("intro_text"));
            const auto& fontPath = std::get<std::string>(gameConfig.at("font"));

            ContentSpace::FontLoader fontLoader{};
            ViewSpace::TextRenderer  textRenderer{};

            if (!introTexts.empty())
            {
                const auto& loadFontResult = fontLoader.loadFont(fontPath);

                if (loadFontResult.error_ != ContentSpace::Definition::ResourceLoadError::None)
                {
                    loadFontResult.showErrorMessage();
                    return 1;
                }

                if (!textRenderer.initializeTextEngine(renderer.getRenderer(), fontLoader.getFont()))
                {
                    return 1;
                }
            }

            ContentSpace::ImageLoader imageLoader{renderer.getRenderer()};

            if (!introImages.empty())
            {
                const auto& loadTextureResult = imageLoader.loadTexture(introImages);

                if (loadTextureResult.error_ != ContentSpace::Definition::ResourceLoadError::None)
                {
                    loadTextureResult.showErrorMessage();
                    return 1;
                }
            }

            GameSpace::GameCore gameCore{
                GameSpace::SimulationCore{},
                InputSpace::InputCore{},
                std::move(renderer),
                std::move(textRenderer),
                {
                    gameConfig.find("health") != gameConfig.end()
                        ? std::get<int>(gameConfig.find("health")->second)
                        : 3,
                    gameConfig.find("score") != gameConfig.end()
                        ? std::get<int>(gameConfig.find("score")->second)
                        : 0,
                    std::move(std::get<std::vector<std::string>>(gameConfig.at("intro_text")))
                },
                std::move(imageLoader),
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
