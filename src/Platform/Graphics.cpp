#include "Detail/RendererImpl.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureStore.h"
#include "Window.h"

#include "Content/ContentResult.h"

#include <glm/glm.hpp>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Uncarved
{
    struct Texture::Impl final
    {
        SDL_Texture* texture_{nullptr};

        explicit Impl(SDL_Texture* texture) : texture_(texture)
        {
        }

        ~Impl()
        {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }

        int getWidth() const noexcept
        {
            return texture_->w;
        }

        int getHeight() const noexcept
        {
            return texture_->h;
        }

        SDL_Texture* getTexture() const noexcept
        {
            return texture_;
        }
    };

    Texture::Texture(std::unique_ptr<Texture::Impl> impl) : impl_(std::move(impl))
    {
    }

    Texture::~Texture() = default;

    Texture::Texture(Texture&& other) noexcept : impl_(std::move(other.impl_))
    {
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            impl_ = std::move(other.impl_);
        }

        return *this;
    }

    int Texture::getWidth() const noexcept
    {
        return impl_->getWidth();
    }

    int Texture::getHeight() const noexcept
    {
        return impl_->getHeight();
    }
} // namespace Uncarved

namespace Uncarved::PlatformSpace
{
    struct Window::Impl final
    {
        SDL_Window* windowPtr_{nullptr};
        WindowSize  windowSize_{};
        std::string windowTitle_{};

        Impl(WindowSize&& windowSize, std::string&& windowTitle)
            : windowSize_(std::move(windowSize))
            , windowTitle_(std::move(windowTitle))
        {
        }

        ~Impl()
        {
            SDL_DestroyWindow(windowPtr_);
            windowPtr_ = nullptr;
        }

        bool initialize()
        {
            if (windowPtr_ != nullptr)
            {
                return true;
            }

            windowPtr_ = SDL_CreateWindow(windowTitle_.c_str(), windowSize_.width_, windowSize_.height_, 0);

            if (windowPtr_ == nullptr)
            {
                std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
                return false;
            }

            return true;
        }

        SDL_Window* getWindowPtr() const noexcept
        {
            return windowPtr_;
        }
    };

    Window::Window(WindowSize&& windowSize, std::string&& windowTitle)
        : impl_(std::make_unique<Impl>(std::move(windowSize), std::move(windowTitle)))
    {
    }

    Window::~Window() = default;

    bool Window::initialize()
    {
        return impl_->initialize();
    }
} // namespace Uncarved::PlatformSpace

namespace Uncarved::PlatformSpace
{
    Renderer::Impl::Impl(Window& outWindow) : outWindow_(outWindow)
    {
    }

    Renderer::Impl::~Impl()
    {
        if (renderer_ != nullptr)
        {
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
        }
    }

    bool Renderer::Impl::initialize()
    {
        if (renderer_ != nullptr)
        {
            return true;
        }

        renderer_ = SDL_CreateRenderer(outWindow_.impl_->getWindowPtr(), nullptr);

        if (renderer_ == nullptr)
        {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
            return false;
        }

        if (!SDL_SetRenderVSync(renderer_, 1))
        {
            std::cerr << "SDL_SetRenderVSync failed: " << SDL_GetError() << '\n';
            SDL_DestroyRenderer(renderer_);
            renderer_ = nullptr;
            return false;
        }

        return true;
    }

    void Renderer::Impl::setClearColor(const Color& color)
    {
        clearColor_ = color;
    }

    bool Renderer::Impl::clear()
    {
        if (!SDL_SetRenderDrawColor(renderer_, clearColor_.r_, clearColor_.g_, clearColor_.b_, SDL_ALPHA_OPAQUE))
        {
            return false;
        }

        return SDL_RenderClear(renderer_);
    }

    bool Renderer::Impl::renderTexture(const Texture& texture)
    {
        if (renderer_ == nullptr || texture.impl_->getTexture() == nullptr)
        {
            return false;
        }

        return SDL_RenderTexture(renderer_, texture.impl_->getTexture(), nullptr, nullptr);
    }

    bool Renderer::Impl::renderTexture(const Texture& texture, const SpriteDrawTransform& spriteDrawTransform)
    {
        if (renderer_ == nullptr || texture.impl_->getTexture() == nullptr)
        {
            return false;
        }

        const bool bFlipHorizontal = spriteDrawTransform.screenSizePixels_.x < 0.0f;
        const bool bFlipVertical = spriteDrawTransform.screenSizePixels_.y < 0.0f;

        const float textureWidth = std::abs(spriteDrawTransform.screenSizePixels_.x);
        const float textureHeight = std::abs(spriteDrawTransform.screenSizePixels_.y);

        const glm::fvec2 normalizedPivot = spriteDrawTransform.normalizedPivotPoint_;
        const float      pivotX =
            bFlipHorizontal ? textureWidth * (1.0f - normalizedPivot.x) : textureWidth * normalizedPivot.x;

        const float pivotY =
            bFlipVertical ? textureHeight * (1.0f - normalizedPivot.y) : textureHeight * normalizedPivot.y;

        SDL_FRect dstRect{
            spriteDrawTransform.screenPositionPixels_.x - pivotX,
            spriteDrawTransform.screenPositionPixels_.y - pivotY,
            textureWidth,
            textureHeight
        };

        SDL_FPoint rotationCenter{pivotX, pivotY};

        auto flipFlag = [&bFlipHorizontal, &bFlipVertical]() {
            if (bFlipHorizontal && bFlipVertical)
            {
                return SDL_FLIP_HORIZONTAL_AND_VERTICAL;
            }

            if (bFlipHorizontal)
            {
                return SDL_FLIP_HORIZONTAL;
            }

            if (bFlipVertical)
            {
                return SDL_FLIP_VERTICAL;
            }

            return SDL_FLIP_NONE;
        };

        return SDL_RenderTextureRotated(
            renderer_,
            texture.impl_->getTexture(),
            nullptr,
            &dstRect,
            spriteDrawTransform.getRotationDegrees(),
            &rotationCenter,
            flipFlag()
        );
    }

    bool Renderer::Impl::present()
    {
        return SDL_RenderPresent(renderer_);
    }

    std::optional<glm::ivec2> Renderer::Impl::getViewportPixels() const
    {
        int height{0};
        int width{0};

        if (!SDL_GetRenderOutputSize(renderer_, &height, &width) || height < 0 || width < 0)
        {
            return std::nullopt;
        }

        return glm::ivec2{height, width};
    }

    SDL_Renderer* Renderer::Impl::getRenderer() const noexcept
    {
        return renderer_;
    }

    Renderer::Renderer(Window& outWindow) : impl_(std::make_unique<Impl>(outWindow))
    {
    }

    Renderer::~Renderer() = default;

    bool Renderer::initialize()
    {
        return impl_->initialize();
    }

    void Renderer::setClearColor(const Color& color)
    {
        impl_->setClearColor(color);
    }

    bool Renderer::clear()
    {
        return impl_->clear();
    }

    bool Renderer::renderTexture(const Texture& texture)
    {
        return impl_->renderTexture(texture);
    }

    bool Renderer::renderTexture(const Texture& texture, const SpriteDrawTransform& spriteDrawTransform)
    {
        return impl_->renderTexture(texture, spriteDrawTransform);
    }

    bool Renderer::present()
    {
        return impl_->present();
    }

    std::optional<glm::ivec2> Renderer::getViewportPixels() const
    {
        return impl_->getViewportPixels();
    }
} // namespace Uncarved::PlatformSpace

namespace Uncarved::PlatformSpace
{
    struct TextureStore::Impl final
    {
        Renderer& outRenderer_;

        using IntroTextureCache = std::vector<Texture>;
        using ActorTextureCache = std::unordered_map<std::string, Texture>;

        using ActorTexturePathCache = std::unordered_map<std::string, std::string>;

        IntroTextureCache introTextureCache_;
        ActorTextureCache actorTextureCache_;

        ActorTexturePathCache actorTexturePathCache_;

        explicit Impl(Renderer& outRenderer) : outRenderer_(outRenderer)
        {
        }

        ContentSpace::ContentResult loadIntroTextures(const std::vector<std::string>& texturePaths)
        {
            if (texturePaths.empty())
            {
                return {};
            }

            IntroTextureCache loadedTextureCache{};

            loadedTextureCache.reserve(texturePaths.size());
            introTextureCache_.reserve(introTextureCache_.size() + texturePaths.size());

            for (const auto& texturePath : texturePaths)
            {
                auto textureImpl = std::make_unique<Texture::Impl>(nullptr);
                textureImpl->texture_ = IMG_LoadTexture(outRenderer_.impl_->getRenderer(), texturePath.c_str());

                Texture texture{std::move(textureImpl)};

                if (texture.impl_->getTexture() == nullptr)
                {
                    loadedTextureCache.clear();

                    return {ContentSpace::ResourceError{
                        "error: failed to load texture " + texturePath + ": " + SDL_GetError(),
                        ContentSpace::ResourceErrorType::LoadFailed
                    }};
                }

                loadedTextureCache.emplace_back(std::move(texture));
            }

            introTextureCache_.clear();
            introTextureCache_ = std::move(loadedTextureCache);

            return {};
        }

        ContentSpace::ContentResult loadActorTextures(const std::vector<std::string>& texturePaths)
        {
            if (texturePaths.empty())
            {
                return {};
            }

            ActorTextureCache loadedTextures{};

            for (const auto& texturePath : texturePaths)
            {
                if (actorTextureCache_.contains(texturePath) || loadedTextures.contains(texturePath))
                {
                    continue;
                }

                auto textureImpl = std::make_unique<Texture::Impl>(nullptr);
                textureImpl->texture_ = IMG_LoadTexture(outRenderer_.impl_->getRenderer(), texturePath.c_str());

                Texture texture{std::move(textureImpl)};

                if (texture.impl_->getTexture() == nullptr)
                {
                    loadedTextures.clear();

                    return {ContentSpace::ResourceError{
                        "error: failed to load texture " + texturePath + ": " + SDL_GetError(),
                        ContentSpace::ResourceErrorType::LoadFailed
                    }};
                }

                loadedTextures.emplace(texturePath, std::move(texture));
            }

            actorTextureCache_.reserve(actorTextureCache_.size() + loadedTextures.size());

            actorTextureCache_.merge(loadedTextures);

            return {};
        }

        const Texture* getIntroTexture(std::size_t index) const noexcept
        {
            if (index >= introTextureCache_.size())
            {
                return nullptr;
            }

            return &introTextureCache_[index];
        }

        void updateActorTexturePathCache(std::string&& textureName, std::string&& texturePath)
        {
            actorTexturePathCache_.emplace(std::move(textureName), std::move(texturePath));
        }

        const Texture* getActorTextureByPath(const std::string& texturePath) const noexcept
        {
            if (texturePath.empty())
            {
                return nullptr;
            }

            if (auto it = actorTextureCache_.find(texturePath); it != actorTextureCache_.end())
            {
                return &(it->second);
            }
            else
            {
                return nullptr;
            }
        }

        const Texture* getActorTextureByName(const std::string& textureName) const noexcept
        {
            if (auto it = actorTexturePathCache_.find(textureName); it != actorTexturePathCache_.end())
            {
                return getActorTextureByPath(it->second);
            }

            return nullptr;
        }

        std::size_t getIntroTextureCount() const noexcept
        {
            return introTextureCache_.size();
        }
    };

    TextureStore::TextureStore(Renderer& outRenderer) : impl_(std::make_unique<Impl>(outRenderer))
    {
    }

    TextureStore::~TextureStore() = default;

    ContentSpace::ContentResult TextureStore::loadIntroTextures(const std::vector<std::string>& texturePaths)
    {
        return impl_->loadIntroTextures(texturePaths);
    }

    ContentSpace::ContentResult TextureStore::loadActorTextures(const std::vector<std::string>& texturePaths)
    {
        return impl_->loadActorTextures(texturePaths);
    }

    const Texture* TextureStore::getIntroTexture(std::size_t index) const noexcept
    {
        return impl_->getIntroTexture(index);
    }

    void TextureStore::registerActorTexturePath(std::string&& textureName, std::string&& texturePath)
    {
        impl_->updateActorTexturePathCache(std::move(textureName), std::move(texturePath));
    }

    const Texture* TextureStore::getActorTextureByName(const std::string& textureName) const noexcept
    {
        return impl_->getActorTextureByName(textureName);
    }

    const Texture* TextureStore::getActorTextureByPath(const std::string& texturePath) const noexcept
    {
        return impl_->getActorTextureByPath(texturePath);
    }

    std::size_t TextureStore::getIntroTextureCount() const noexcept
    {
        return impl_->getIntroTextureCount();
    }
} // namespace Uncarved::PlatformSpace
