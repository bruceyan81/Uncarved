#pragma once

#include "Content/DataDefinition.h"

#include <string>
#include <cstddef>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace Uncarved::ContentSpace
{
    class ImageLoader final
    {
    public:
        explicit ImageLoader(SDL_Renderer*);

        ImageLoader(const ImageLoader&) = delete;
        ImageLoader& operator=(const ImageLoader&) = delete;

        ImageLoader(ImageLoader&&) noexcept = default;
        ImageLoader& operator=(ImageLoader&&) noexcept = delete;

        ~ImageLoader();

        ContentSpace::Definition::ResourceLoadResult loadTexture(const std::vector<std::string>& list);

        SDL_Texture* getTexture(std::size_t index) const noexcept;

        std::size_t getTextureCount() const noexcept
        {
            return textureCache_.size();
        }

    private:
        SDL_Renderer*             renderer_{nullptr};
        std::vector<SDL_Texture*> textureCache_;
    };
} // namespace Uncarved::ContentSpace
