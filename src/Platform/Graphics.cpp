#include "Detail/RendererImpl.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureStore.h"
#include "Window.h"

#include "Content/ContentResult.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <iostream>
#include <memory>
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
    class Window::Impl final
    {
    public:
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

    private:
        SDL_Window* windowPtr_{nullptr};
        WindowSize  windowSize_{};
        std::string windowTitle_{};
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

    bool Renderer::Impl::renderTexture(const Texture& texture, const SpriteTransform& spriteTransform)
    {
        if (renderer_ == nullptr || texture.impl_->getTexture() == nullptr)
        {
            return false;
        }

        const float textureWidth = spriteTransform.scale_.x * (texture.getWidth());
        const float textureHeight = spriteTransform.scale_.y * (texture.getHeight());

        SDL_FRect dstRect{spriteTransform.position_.x, spriteTransform.position_.y, textureWidth, textureHeight};

        const auto normalizedPivot = spriteTransform.normalizedPivot_;

        SDL_FPoint rotationCenter{};

        if (normalizedPivot.has_value())
        {
            rotationCenter.x = textureWidth * (*normalizedPivot).x;
            rotationCenter.y = textureHeight * (*normalizedPivot).y;
        }

        return SDL_RenderTextureRotated(
            renderer_,
            texture.impl_->getTexture(),
            nullptr,
            &dstRect,
            spriteTransform.getRotationDegrees(),
            normalizedPivot ? &rotationCenter : nullptr,
            SDL_FLIP_NONE
        );
    }

    bool Renderer::Impl::present()
    {
        return SDL_RenderPresent(renderer_);
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

    bool Renderer::renderTexture(const Texture& texture, const SpriteTransform& spriteTransform)
    {
        return impl_->renderTexture(texture, spriteTransform);
    }

    bool Renderer::present()
    {
        return impl_->present();
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
