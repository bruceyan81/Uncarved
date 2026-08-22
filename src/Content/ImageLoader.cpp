#include "ImageLoader.h"

#include <SDL3_image/SDL_image.h>

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

    ContentSpace::Definition::ResourceLoadResult ImageLoader::loadTexture(const std::vector<std::string>& texturePath)
    {
        if (texturePath.empty())
        {
            return {};
        }

        textureCache_.reserve(texturePath.size());

        for (const auto& name : texturePath)
        {
            SDL_Texture* texture = IMG_LoadTexture(renderer_, name.c_str());

            if (texture == nullptr)
            {
                return {ContentSpace::Definition::ResourceLoadError::MissingTexture, "Error reading texture."};
            }

            textureCache_.push_back(texture);
        }

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
