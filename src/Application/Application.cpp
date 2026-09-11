#include "Application.h"

#include "Game/Game.h"
#include "Input/Input.h"
#include "Platform/EventPump.h"
#include "Platform/FontLoader.h"
#include "Platform/PlatformRuntime.h"
#include "Platform/Renderer.h"
#include "Platform/TextRenderer.h"
#include "Platform/TextRuntime.h"
#include "Platform/TextureStore.h"
#include "Platform/Window.h"

#include <cstdint>
#include <iostream>

namespace Uncarved::ApplicationSpace
{
    ApplicationCore::ApplicationCore()
    {
        applicationState_ = ApplicationState::Init;
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

        return 0;
    }

    int ApplicationCore::launch()
    {
        if (initializeApplication() == 0)
        {
            applicationState_ = ApplicationState::Running;

            PlatformSpace::PlatformRuntime platformRuntime{};

            if (!platformRuntime.initialize())
            {
                return 1;
            }

            const auto& gameConfig = gameContentLoader_.getGameConfig();
            const auto& renderingConfig = gameContentLoader_.getRenderingConfig();
            const auto& introConfig = gameContentLoader_.getIntroConfig();

            PlatformSpace::Window window{
                {renderingConfig.xResolution_, renderingConfig.yResolution_},
                std::string{gameConfig.gameTitle_}
            };

            if (!window.initialize())
            {
                return 1;
            }

            PlatformSpace::Renderer renderer{window};

            if (!renderer.initialize())
            {
                return 1;
            }

            renderer.setClearColor(
                {
                    static_cast<std::uint8_t>(renderingConfig.clearColorR_),
                    static_cast<std::uint8_t>(renderingConfig.clearColorG_),
                    static_cast<std::uint8_t>(renderingConfig.clearColorB_)
                }
            );

            const auto& fontPath = gameConfig.fontPath_;
            const auto& introImages = introConfig.introImages_;
            const auto& introTexts = introConfig.introText_;

            PlatformSpace::TextRuntime textRuntime{};

            if (!introTexts.empty())
            {
                if (!textRuntime.initialize())
                {
                    return 1;
                }
            }

            PlatformSpace::FontLoader fontLoader{textRuntime};

            if (!introTexts.empty())
            {
                const auto& loadFontResult = fontLoader.loadFont(fontPath);

                if (!loadFontResult.isSucceeded())
                {
                    std::cerr << loadFontResult.getErrorMessage();
                    return 1;
                }
            }

            PlatformSpace::TextRenderer textRenderer{textRuntime, fontLoader.getFont(), renderer};

            if (!introTexts.empty() && !textRenderer.initialize())
            {
                return 1;
            }

            PlatformSpace::TextureStore textureStore{renderer};

            if (!introImages.empty())
            {
                const auto& loadTextureResult = textureStore.loadIntroTextures(introImages);

                if (!loadTextureResult.isSucceeded())
                {
                    std::cerr << loadTextureResult.getErrorMessage();
                    return 1;
                }
            }

            PlatformSpace::EventPump eventPump{};
            InputSpace::InputCore    inputCore{eventPump};

            GameSpace::GameCore gameCore{
                {gameConfig.health_, gameConfig.score_, introConfig.introText_},
                GameSpace::SimulationCore{},
                inputCore,
                renderer,
                textRenderer,
                textureStore,
                gameContentLoader_
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
