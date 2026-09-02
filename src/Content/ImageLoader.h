#pragma once

#include "ContentResult.h"

#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace Uncarved::ContentSpace
{
    struct SDLTextureDeleter
    {
        void operator()(SDL_Texture* texture) const noexcept
        {
            SDL_DestroyTexture(texture);
        }
    };

    class ImageLoader final
    {
    public:
        explicit ImageLoader(SDL_Renderer*);

        ImageLoader(const ImageLoader&) = delete;
        ImageLoader& operator=(const ImageLoader&) = delete;

        ImageLoader(ImageLoader&&) noexcept = default;
        ImageLoader& operator=(ImageLoader&&) noexcept = delete;

        ~ImageLoader();

        ContentResult loadIntroTexture(const std::vector<std::string>& texturePaths);

        ContentResult loadActorTexture(const std::vector<std::string>& texturePaths);

        SDL_Texture* getIntroTexture(std::size_t index) const noexcept;

        void updateActorTexturePathCache(std::string&& textureName, std::string&& texturePath);

        SDL_Texture* getActorTextureByName(const std::string& textureName) const noexcept;

        SDL_Texture* getActorTextureByPath(const std::string& texturePath) const noexcept;

        std::size_t getIntroTextureCount() const noexcept
        {
            return introTextureCache_.size();
        }

    private:
        using IntroTexturePtr = std::vector<std::unique_ptr<SDL_Texture, SDLTextureDeleter>>;
        using ActorTexturePtr = std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>>;

        using ActorTexturePathCache = std::unordered_map<std::string, std::string>;

        SDL_Renderer*   renderer_{nullptr};

        IntroTexturePtr introTextureCache_;
        ActorTexturePtr actorTextureCache_;

        ActorTexturePathCache actorTexturePathCache_{};
    };
} // namespace Uncarved::ContentSpace
