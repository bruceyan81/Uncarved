#include "Runtime.h"

#include "Application/Application.h"
#include "Application/RuntimeContext.h"
#include "Content/ContentResult.h"
#include "Content/GameContentLoader.h"
#include "Input/InputSystem.h"
#include "Logging/Log.h"
#include "Platform/EventPump.h"
#include "Platform/PlatformEvent.h"
#include "Platform/Renderer.h"
#include "Platform/TextureStore.h"
#include "Time/AppTime.h"
#include "View/SceneRenderer.h"
#include "View/Sprite.h"
#include "View/SpriteCatalog.h"

#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Uncarved::GameSpace
{
    RuntimeCore::RuntimeCore(
        ViewSpace::Camera2D&& camera2d,

        TimeSpace::AppTime&              outAppTime,
        InputSpace::InputSystem&         outInputSystem,
        PlatformSpace::EventPump&        outEventPump,
        PlatformSpace::Renderer&         outRenderer,
        PlatformSpace::TextureStore&     outTextureStore,
        ViewSpace::SceneRenderer&        outSceneRenderer,
        ContentSpace::GameContentLoader& outGameContentLoader,
        ApplicationSpace::Application&   outApplication
    )
        : camera2d_(std::move(camera2d))

        , outAppTime_(outAppTime)
        , outInputSystem_(outInputSystem)
        , outEventPump_(outEventPump)
        , outRenderer_(outRenderer)
        , outTextureStore_(outTextureStore)
        , outSceneRenderer_(outSceneRenderer)
        , outGameContentLoader_(outGameContentLoader)
        , outApplication_(outApplication)
    {
        runtimeState_ = RuntimeState::Init;
    }

    int RuntimeCore::launch()
    {
        if (runtimeState_ != RuntimeState::Init)
        {
            return 1;
        }

        const auto loadSceneResult = loadScene(outGameContentLoader_.getGameConfig().initialSceneName_);

        if (!loadSceneResult.isSucceeded())
        {
            UC_LOG(gLogContent, LogVerbosity::Error, "{}", loadSceneResult.getErrorMessage());
            return 1;
        }

        runtimeState_ = RuntimeState::Running;

        ApplicationSpace::RuntimeContext runtimeContext{
            world_,
            camera2d_,
            outInputSystem_,
            outAppTime_,
            ViewSpace::SpriteCatalog{outGameContentLoader_.getSprites()}
        };

        outApplication_.onInitialize(runtimeContext);

        while (runtimeState_ == RuntimeState::Running)
        {
            outAppTime_.update();
            world_.updateTime(outAppTime_.getDeltaTime());

            outInputSystem_.beginFrame();

            PlatformEvent platformEvent{};

            while (outEventPump_.pollEvent(platformEvent))
            {
                switch (platformEvent.type_)
                {
                    case PlatformEventType::Quit:
                    case PlatformEventType::WindowCloseRequested:
                        commandBuffer_.submit(ExitRuntimeCommand{});
                        break;

                    case PlatformEventType::KeyPressed:
                    case PlatformEventType::KeyReleased:
                    case PlatformEventType::KeyRepeat:
                        outInputSystem_.processPlatformEvent(platformEvent);
                        break;
                    default:
                        break;
                }
            }

            commitCommands();

            if (runtimeState_ != RuntimeState::Running)
            {
                break;
            }

            outApplication_.onUpdate(runtimeContext);

            if (!outRenderer_.clear() || !outSceneRenderer_.render(world_, camera2d_) || !outRenderer_.present())
            {
                outApplication_.onShutdown(runtimeContext);
                return 1;
            }
        }

        outApplication_.onShutdown(runtimeContext);
        return runtimeState_ == RuntimeState::End ? 0 : 1;
    }

    ContentSpace::ContentResult RuntimeCore::loadScene(std::string_view sceneName)
    {
        const auto checkSceneResult = outGameContentLoader_.checkSceneResource(sceneName);

        if (!checkSceneResult.isSucceeded())
        {
            return checkSceneResult;
        }

        const auto loadSceneResult = outGameContentLoader_.loadSceneResource(sceneName);

        if (!loadSceneResult.isSucceeded())
        {
            return loadSceneResult;
        }

        auto nextWorld =
            world_.createReplacement(outGameContentLoader_.getDefinitionalActors(), outGameContentLoader_.getSprites());

        if (!nextWorld.has_value())
        {
            return {ContentSpace::ResourceError{
                "error: failed to resolve actor sprites.",
                ContentSpace::ResourceErrorType::LoadFailed
            }};
        }

        const auto loadWorldTexturesResult = loadWorldTextures(*nextWorld);

        if (!loadWorldTexturesResult.isSucceeded())
        {
            return loadWorldTexturesResult;
        }

        world_ = std::move(*nextWorld);
        outGameContentLoader_.releaseLoadData();
        return {};
    }

    ContentSpace::ContentResult RuntimeCore::loadWorldTextures(const World& world)
    {
        const auto&              actors = world.getActors();
        std::vector<std::string> texturePaths{};

        texturePaths.reserve(actors.size());

        for (const auto& actor : actors)
        {
            auto& runtimeSpriteVisual2d = actor.getRuntimeSpriteVisual2d();

            const ViewSpace::Sprite* actorSprite = runtimeSpriteVisual2d.getSprite();

            if (actorSprite == nullptr)
            {
                continue;
            }

            const std::string& textureName = actorSprite->getTextureName();
            std::string        path = outGameContentLoader_.createActorTexturePath(textureName);

            if (!path.empty())
            {
                texturePaths.push_back(path);
                outTextureStore_.registerActorTexturePath(std::string{textureName}, std::move(path));
            }
        }

        return outTextureStore_.loadActorTextures(texturePaths);
    }

    void RuntimeCore::commitCommands() noexcept
    {
        for (const auto& command : commandBuffer_.getCommands())
        {
            std::visit(
                [this](const auto& targetCommand)
                {
                    commitCommand(targetCommand);
                },
                command
            );
        }

        commandBuffer_.clearCommands();
    }

    void RuntimeCore::commitCommand(const ExitRuntimeCommand& command) noexcept
    {
        (void)command;
        runtimeState_ = RuntimeState::End;
    }
} // namespace Uncarved::GameSpace
