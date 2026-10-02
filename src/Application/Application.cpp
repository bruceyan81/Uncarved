#include "Application.h"

#include "Content/ContentResult.h"
#include "Game/Runtime.h"
#include "Input/InputSystem.h"
#include "Platform/EventPump.h"
#include "Platform/PlatformRuntime.h"
#include "Platform/Renderer.h"
#include "Platform/TextureStore.h"
#include "Platform/Window.h"
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
        if (initializeApplication() != 0)
        {
            return 1;
        }

        applicationState_ = ApplicationState::Running;
        PlatformSpace::PlatformRuntime platformRuntime{};

        if (!platformRuntime.initialize())
        {
            return 1;
        }

        const auto& gameConfig = gameContentLoader_.getGameConfig();
        const auto& renderingConfig = gameContentLoader_.getRenderingConfig();

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

        PlatformSpace::TextureStore textureStore{renderer};

        auto camera2d = ViewSpace::Camera2D::createCamera2D(renderingConfig.cameraOrthoWidth_);

        if (!camera2d.has_value())
        {
            return 1;
        }

        ViewSpace::SceneRenderer sceneRenderer{renderer, textureStore};

        PlatformSpace::EventPump eventPump{};
        InputSpace::InputSystem inputSystem{};

        GameSpace::RuntimeCore runtimeCore{
            std::move(*camera2d),
            appTime_,
            inputSystem,
            eventPump,
            renderer,
            textureStore,
            sceneRenderer,
            gameContentLoader_
        };

        if (runtimeCore.launch() == 0)
        {
            applicationState_ = ApplicationState::End;
            return 0;
        }

        return 1;
    }
} // namespace Uncarved::ApplicationSpace
