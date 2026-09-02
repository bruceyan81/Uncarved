#include "ImageLoader.h"

#include <string>
#include <utility>
#include <vector>

namespace Uncarved::ContentSpace
{
    Uncarved::ContentSpace::ImageLoader::ImageLoader(SDL_Renderer* rendererPtr) : renderer_(rendererPtr)
    {
    }

    ImageLoader::~ImageLoader()
    {
        renderer_ = nullptr;

        introTextureCache_.clear();
        actorTextureCache_.clear();
    }

    ContentResult ImageLoader::loadIntroTexture(const std::vector<std::string>& texturePaths)
    {
        if (texturePaths.empty())
        {
            return {};
        }

        IntroTexturePtr loadedTextures{};

        loadedTextures.reserve(texturePaths.size());
        introTextureCache_.reserve(introTextureCache_.size() + texturePaths.size());

        for (const auto& texturePath : texturePaths)
        {
            SDL_Texture* texture = IMG_LoadTexture(renderer_, texturePath.c_str());

            if (texture == nullptr)
            {
                return {ResourceError{
                    "error: failed to load texture " + texturePath + ": " + SDL_GetError(),
                    ResourceErrorType::LoadFailed
                }};
            }

            loadedTextures.emplace_back(texture);
        }

        introTextureCache_.clear();
        introTextureCache_ = std::move(loadedTextures);

        return {};
    }

    ContentResult ImageLoader::loadActorTexture(const std::vector<std::string>& texturePaths)
    {
        if (texturePaths.empty())
        {
            return {};
        }

        ActorTexturePtr loadedTextures{};

        for (const auto& texturePath : texturePaths)
        {
            if (actorTextureCache_.contains(texturePath) || loadedTextures.contains(texturePath))
            {
                continue;
            }

            std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture =
                std::unique_ptr<SDL_Texture, SDLTextureDeleter>(IMG_LoadTexture(renderer_, texturePath.c_str()));

            if (texture == nullptr)
            {
                loadedTextures.clear();

                return {ResourceError{
                    "error: failed to load texture " + texturePath + ": " + SDL_GetError(),
                    ResourceErrorType::LoadFailed
                }};
            }

            loadedTextures.emplace(texturePath, std::move(texture));
        }

        actorTextureCache_.reserve(actorTextureCache_.size() + loadedTextures.size());

        actorTextureCache_.merge(loadedTextures);

        return {};
    }

    SDL_Texture* ImageLoader::getIntroTexture(std::size_t index) const noexcept
    {
        if (index >= introTextureCache_.size())
        {
            return nullptr;
        }

        return introTextureCache_[index].get();
    }

    void ImageLoader::updateActorTexturePathCache(std::string&& textureName, std::string&& texturePath)
    {
        actorTexturePathCache_.emplace(std::move(textureName), std::move(texturePath));
    }

    SDL_Texture* ImageLoader::getActorTextureByName(const std::string& textureName) const noexcept
    {
        if (auto it = actorTexturePathCache_.find(textureName); it != actorTexturePathCache_.end())
        {
            return getActorTextureByPath(it->second);
        }

        return nullptr;
    }

    SDL_Texture* ImageLoader::getActorTextureByPath(const std::string& texturePath) const noexcept
    {
        SDL_Texture* actorTexturePtr = nullptr;

        if (texturePath.empty())
        {
            return actorTexturePtr;
        }

        if (auto it = actorTextureCache_.find(texturePath); it != actorTextureCache_.end())
        {
            actorTexturePtr = it->second.get();
        }

        return actorTexturePtr;
    }
} // namespace Uncarved::ContentSpace
