#include "Application.h"

#include "Content/ContentResult.h"
#include "Game/Game.h"
#include "Input/Input.h"
#include "Platform/Audio/AudioDevice.h"
#include "Platform/Audio/SoundSource.h"
#include "Platform/Audio/SoundWaveStore.h"
#include "Platform/EventPump.h"
#include "Platform/FontLoader.h"
#include "Platform/PlatformRuntime.h"
#include "Platform/Renderer.h"
#include "Platform/TextRenderer.h"
#include "Platform/TextRuntime.h"
#include "Platform/TextureStore.h"
#include "Platform/Window.h"
#include "Time/AppTime.h"
#include "View/Camera2D.h"
#include "View/SceneRenderer.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

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

        if (!renderingConfigCheckResult.isSucceeded())
        {
            std::cerr << renderingConfigCheckResult.getErrorMessage();
            return 1;
        }

        const auto renderingConfigLoadResult = gameContentLoader_.loadRenderingConfig();

        if (!renderingConfigLoadResult.isSucceeded())
        {
            std::cerr << renderingConfigLoadResult.getErrorMessage();
            return 1;
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

        const auto spritesCheckResult = gameContentLoader_.checkSprites();

        if (!spritesCheckResult.isSucceeded())
        {
            std::cerr << spritesCheckResult.getErrorMessage();
            return 1;
        }

        const auto spritesLoadResult = gameContentLoader_.loadSprites();

        if (!spritesLoadResult.isSucceeded())
        {
            std::cerr << spritesLoadResult.getErrorMessage();
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

            PlatformSpace::AudioDevice audioDevice{};

            if (!audioDevice.initialize())
            {
                return 1;
            }

            PlatformSpace::SoundWaveStore soundWaveStore{audioDevice};

            PlatformSpace::SoundSource soundSource{audioDevice};

            if (!soundSource.initialize())
            {
                return 1;
            }

            auto camera2d = ViewSpace::Camera2D::createCamera2D(renderingConfig.cameraOrthoWidth_);

            if (!camera2d.has_value())
            {
                return 1;
            }

            ViewSpace::SceneRenderer sceneRenderer{renderer, textureStore};

            GameSpace::GameCore gameCore{
                {gameConfig.health_, gameConfig.score_, introConfig.introText_},
                GameSpace::SimulationCore{},
                std::move(*camera2d),
                appTime_,
                inputCore,
                renderer,
                textRenderer,
                textureStore,
                sceneRenderer,
                soundSource,
                soundWaveStore,
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
