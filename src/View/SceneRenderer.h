#pragma once

#include <vector>

namespace Uncarved
{
    namespace GameSpace
    {
        class World;
    }

    namespace PlatformSpace
    {
        class Renderer;
        class TextureStore;
    } // namespace PlatformSpace

    namespace ViewSpace
    {
        class Camera2D;
    } // namespace ViewSpace
} // namespace Uncarved

namespace Uncarved::ViewSpace
{
    class SceneRenderer final
    {
    public:
        SceneRenderer(PlatformSpace::Renderer& outRenderer, PlatformSpace::TextureStore& outTextureStore);
        ~SceneRenderer();

        bool render(const GameSpace::World& outWorld, const Camera2D& outCamera2d);

    private:
        struct SpriteRecord;
        std::vector<SpriteRecord> records_;

        PlatformSpace::Renderer&     outRenderer_;
        PlatformSpace::TextureStore& outTextureStore_;
    };

} // namespace Uncarved::ViewSpace
