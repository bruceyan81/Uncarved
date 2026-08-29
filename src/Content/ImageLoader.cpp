#include "ImageLoader.h"

#include <SDL3_image/SDL_image.h>

#include <string>
#include <vector>

namespace Uncarved::ContentSpace
{
    Uncarved::ContentSpace::ImageLoader::ImageLoader(SDL_Renderer* rendererPtr) : renderer_(rendererPtr)
    {
    }

    ImageLoader::~ImageLoader()
    {
        renderer_ = nullptr;

        for (auto& texture : textureCache_)
        {
            SDL_DestroyTexture(texture);
        }
    }

    ContentResult ImageLoader::loadTexture(const std::vector<std::string>& texturePaths)
    {
        if (texturePaths.empty())
        {
            return {};
        }

        std::vector<SDL_Texture*> loadedTextures{};
        loadedTextures.reserve(texturePaths.size());
        textureCache_.reserve(textureCache_.size() + texturePaths.size());

        for (const auto& texturePath : texturePaths)
        {
            SDL_Texture* texture = IMG_LoadTexture(renderer_, texturePath.c_str());

            if (texture == nullptr)
            {
                for (auto* loadedTexture : loadedTextures)
                {
                    SDL_DestroyTexture(loadedTexture);
                }

                return {ResourceError{
                    "error: failed to load texture " + texturePath + ": " + SDL_GetError(),
                    ResourceErrorType::LoadFailed
                }};
            }

            loadedTextures.push_back(texture);
        }

        textureCache_.insert(textureCache_.end(), loadedTextures.begin(), loadedTextures.end());

        return {};
    }

    SDL_Texture* ImageLoader::getTexture(std::size_t index) const noexcept
    {
        if (index >= textureCache_.size())
        {
            return nullptr;
        }

        return textureCache_[index];
    }
} // namespace Uncarved::ContentSpace
