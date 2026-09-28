#pragma once

#include "CommandBuffer.h"
#include "World.h"

#include "View/Camera2D.h"

#include <string_view>

namespace Uncarved::ContentSpace
{
    struct ContentResult;
    class GameContentLoader;
} // namespace Uncarved::ContentSpace

namespace Uncarved::InputSpace
{
    class InputCore;
} // namespace Uncarved::InputSpace

namespace Uncarved::TimeSpace
{
    class AppTime;
} // namespace Uncarved::TimeSpace

namespace Uncarved::PlatformSpace
{
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
            ViewSpace::Camera2D&&            camera2d,

            TimeSpace::AppTime&              outAppTime,
            InputSpace::InputCore&           outInputCore,
            PlatformSpace::Renderer&         outRenderer,
            PlatformSpace::TextureStore&     outTextureStore,
            ViewSpace::SceneRenderer&        outSceneRenderer,
            ContentSpace::GameContentLoader& outGameContentLoader
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
        InputSpace::InputCore&           outInputCore_;
        PlatformSpace::Renderer&         outRenderer_;
        PlatformSpace::TextureStore&     outTextureStore_;
        ViewSpace::SceneRenderer&        outSceneRenderer_;
        ContentSpace::GameContentLoader& outGameContentLoader_;

        ContentSpace::ContentResult loadScene(std::string_view sceneName);
        ContentSpace::ContentResult loadWorldTextures(const World& world);

        void                        commitCommands() noexcept;
        void                        commitCommand(const ExitRuntimeCommand& command) noexcept;
    };
} // namespace Uncarved::GameSpace
