#pragma once

#include "CommandBuffer.h"
#include "World.h"

#include "View/Camera2D.h"

#include <memory>
#include <string_view>

namespace Uncarved::ApplicationSpace
{
    class Application;
}

namespace Uncarved::ContentSpace
{
    struct ContentResult;
    class GameContentLoader;
} // namespace Uncarved::ContentSpace

namespace Uncarved::InputSpace
{
    class InputSystem;
} // namespace Uncarved::InputSpace

namespace Uncarved::TimeSpace
{
    class AppTime;
} // namespace Uncarved::TimeSpace

namespace Uncarved::PlatformSpace
{
    class EventPump;
    class Renderer;
    class TextureStore;
} // namespace Uncarved::PlatformSpace

namespace Uncarved::ViewSpace
{
    class SceneRenderer;
} // namespace Uncarved::ViewSpace

namespace Uncarved::GameSpace
{
    enum class RuntimeState
    {
        None,
        Init,
        Running,
        End,
        Count
    };

    class RuntimeCore final
    {
    public:
        RuntimeCore(
            ViewSpace::Camera2D&& camera2d,

            TimeSpace::AppTime&              outAppTime,
            InputSpace::InputSystem&         outInputSystem,
            PlatformSpace::EventPump&        outEventPump,
            PlatformSpace::Renderer&         outRenderer,
            PlatformSpace::TextureStore&     outTextureStore,
            ViewSpace::SceneRenderer&        outSceneRenderer,
            ContentSpace::GameContentLoader& outGameContentLoader,
            ApplicationSpace::Application&   outApplication
        );

        RuntimeCore(const RuntimeCore&) = delete;
        RuntimeCore& operator=(const RuntimeCore&) = delete;

        RuntimeCore(RuntimeCore&&) noexcept = delete;
        RuntimeCore& operator=(RuntimeCore&&) noexcept = delete;

        ~RuntimeCore() = default;

        int launch();

    private:
        RuntimeState        runtimeState_{RuntimeState::None};
        World               world_{};
        CommandBuffer       commandBuffer_{};
        ViewSpace::Camera2D camera2d_;

        TimeSpace::AppTime&              outAppTime_;
        InputSpace::InputSystem&         outInputSystem_;
        PlatformSpace::EventPump&        outEventPump_;
        PlatformSpace::Renderer&         outRenderer_;
        PlatformSpace::TextureStore&     outTextureStore_;
        ViewSpace::SceneRenderer&        outSceneRenderer_;
        ContentSpace::GameContentLoader& outGameContentLoader_;
        ApplicationSpace::Application&   outApplication_;

        ContentSpace::ContentResult loadScene(std::string_view sceneName);
        ContentSpace::ContentResult loadWorldTextures(const World& world);

        void commitCommands() noexcept;
        void commitCommand(const ExitRuntimeCommand& command) noexcept;
    };
} // namespace Uncarved::GameSpace
