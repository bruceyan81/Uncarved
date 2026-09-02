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
#include <utility>

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
        const auto resourceDirectoryResult = gameContentLoader_.checkResourceDirectory();

        if (!resourceDirectoryResult.isSucceeded())
        {
            std::cerr << resourceDirectoryResult.getErrorMessage();
            return 1;
        }

        const auto gameConfigCheckResult = gameContentLoader_.checkGameConfig();

        if (!gameConfigCheckResult.isSucceeded())
        {
            std::cerr << gameConfigCheckResult.getErrorMessage();
            return 1;
        }

        const auto gameConfigLoadResult = gameContentLoader_.loadGameConfig();

        if (!gameConfigLoadResult.isSucceeded())
        {
            std::cerr << gameConfigLoadResult.getErrorMessage();
            return 1;
        }

        const auto renderingConfigCheckResult = gameContentLoader_.checkRenderingConfig();

        if (renderingConfigCheckResult.isSucceeded())
        {
            const auto renderingConfigLoadResult = gameContentLoader_.loadRenderingConfig();

            if (!renderingConfigLoadResult.isSucceeded())
            {
                std::cout << renderingConfigLoadResult.getErrorMessage();
            }
        }
        else
        {
            std::cout << renderingConfigCheckResult.getErrorMessage();
        }

        const auto introConfigCheckResult = gameContentLoader_.checkIntroConfig();

        if (!introConfigCheckResult.isSucceeded())
        {
            std::cerr << introConfigCheckResult.getErrorMessage();
            return 1;
        }

        const auto introConfigLoadResult = gameContentLoader_.loadIntroConfig();

        if (!introConfigLoadResult.isSucceeded())
        {
            std::cerr << introConfigLoadResult.getErrorMessage();
            return 1;
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
        if (initializeApplication() == 0)
        {
            applicationState_ = ApplicationState::Running;

            const auto& gameConfig = gameContentLoader_.getGameConfig();
            const auto& renderingConfig = gameContentLoader_.getRenderingConfig();
            const auto& introConfig = gameContentLoader_.getIntroConfig();

            WindowManager windowManager{
                {renderingConfig.xResolution_, renderingConfig.yResolution_},
                gameConfig.gameTitle_
            };

            if (!windowManager.initializeWindow())
            {
                return 1;
            }

            ViewSpace::Renderer renderer{};

            bool bRendererInitResult = renderer.initializeRenderer(
                windowManager.getWindow(),
                renderingConfig.clearColorR_,
                renderingConfig.clearColorG_,
                renderingConfig.clearColorB_
            );

            if (!bRendererInitResult)
            {
                return 1;
            }

            ContentSpace::FontLoader fontLoader{};
            ViewSpace::TextRenderer  textRenderer{};

            const auto& fontPath = gameConfig.fontPath_;
            const auto& introImages = introConfig.introImages_;
            const auto& introTexts = introConfig.introText_;

            if (!introTexts.empty())
            {
                const auto& loadFontResult = fontLoader.loadFont(fontPath);

                if (!loadFontResult.isSucceeded())
                {
                    std::cerr << loadFontResult.getErrorMessage();
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
                const auto& loadTextureResult = imageLoader.loadIntroTexture(introImages);

                if (!loadTextureResult.isSucceeded())
                {
                    std::cerr << loadTextureResult.getErrorMessage();
                    return 1;
                }
            }

            GameSpace::GameCore gameCore{
                GameSpace::SimulationCore{},
                InputSpace::InputCore{},
                std::move(renderer),
                std::move(textRenderer),
                {
                    gameConfig.health_,
                    gameConfig.score_,
                    introConfig.introText_
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
