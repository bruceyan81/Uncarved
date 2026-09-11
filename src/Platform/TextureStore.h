#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Uncarved
{
    class Texture;

    namespace ContentSpace
    {
        struct ContentResult;
    }
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    class Renderer;

    class TextureStore final
    {
    public:
        explicit TextureStore(Renderer& outRenderer);

        ~TextureStore();

        ContentSpace::ContentResult loadIntroTextures(const std::vector<std::string>& texturePaths);

        ContentSpace::ContentResult loadActorTextures(const std::vector<std::string>& texturePaths);

        const Texture* getIntroTexture(std::size_t index) const noexcept;

        void registerActorTexturePath(std::string&& textureName, std::string&& texturePath);

        const Texture* getActorTextureByName(const std::string& textureName) const noexcept;

        const Texture* getActorTextureByPath(const std::string& texturePath) const noexcept;

        std::size_t getIntroTextureCount() const noexcept;

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };
} // namespace Uncarved::PlatformSpace
