#include "SceneRenderer.h"

#include "Camera2D.h"

#include "Game/World.h"
#include "Platform/Renderer.h"
#include "Platform/Texture.h"
#include "Platform/TextureStore.h"

#include <algorithm>
#include <tuple>
#include <vector>

namespace Uncarved::ViewSpace
{
    struct SceneRenderer::SpriteRecord final
    {
        PlatformSpace::SpriteDrawTransform transform_;
        const Texture*                     outTexture_;
        int                                zOrder_;
        ObjectSpace::ActorId               actorId_;
    };

    SceneRenderer::SceneRenderer(PlatformSpace::Renderer& outRenderer, PlatformSpace::TextureStore& outTextureStore)
        : outRenderer_(outRenderer)
        , outTextureStore_(outTextureStore)
    {
    }

    SceneRenderer::~SceneRenderer() = default;

    bool SceneRenderer::render(const GameSpace::World& outWorld, const Camera2D& outCamera2d)
    {
        const auto viewportPixels = outRenderer_.getViewportPixels();

        if (!viewportPixels.has_value())
        {
            return false;
        }

        if (viewportPixels->x == 0 || viewportPixels->y == 0)
        {
            return true;
        }

        const float viewportPixelsPerWU = outCamera2d.getViewportPixelsPerWorldUnit(viewportPixels->x);
        const auto& actors = outWorld.getActors();

        records_.clear();
        records_.reserve(actors.size());

        for (const auto& actor : actors)
        {
            const auto* sprite = actor.getSprite();

            if (sprite == nullptr)
            {
                continue;
            }

            const Texture* texture = outTextureStore_.getActorTextureByName(sprite->getTextureName());

            if (texture == nullptr)
            {
                return false;
            }

            const auto& transform2d = actor.getTransform2d();

            const glm::fvec2 viewportPositionPixels =
                outCamera2d.worldToViewport(transform2d.position_, *viewportPixels);

            const float screenSizeWidth = texture->getWidth() / actor.getSprite()->getTexturePixelsPerWorldUnit()
                * transform2d.scale_.x * viewportPixelsPerWU;

            const float screenSizeHeight = texture->getHeight() / actor.getSprite()->getTexturePixelsPerWorldUnit()
                * transform2d.scale_.y * viewportPixelsPerWU;

            records_.emplace_back(
                SpriteRecord
                {
                    PlatformSpace::SpriteDrawTransform
                    {
                        sprite->getNormalizedPivot(),
                        viewportPositionPixels,
                        glm::fvec2{screenSizeWidth, screenSizeHeight},
                        transform2d.rotationRadians_
                    },
                    texture,
                    actor.getZOrder(),
                    actor.getId()
                }
            );
        }

        std::ranges::sort(
            records_,
            [](const SpriteRecord& left, const SpriteRecord& right)
            {
                return std::tie(left.zOrder_, left.actorId_) < std::tie(right.zOrder_, right.actorId_);
            }
        );

        for (const auto& record : records_)
        {
            if (!outRenderer_.renderTexture(*record.outTexture_, record.transform_))
            {
                return false;
            }
        }

        return true;
    }
} // namespace Uncarved::ViewSpace
