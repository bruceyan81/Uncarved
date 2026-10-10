#include "SandboxApplication.h"

#include "Application/RuntimeContext.h"
#include "Game/World.h"
#include "Input/InputSystem.h"
#include "Input/Key.h"
#include "Object/Actor.h"
#include "Time/AppTime.h"
#include "View/SpriteCatalog.h"

#include <glm/geometric.hpp>

#include <cmath>
#include <memory>

namespace Uncarved::ApplicationSpace
{
    std::unique_ptr<Application> createApplication()
    {
        return std::make_unique<Sandbox::SandboxApplication>();
    }
} // namespace Uncarved::ApplicationSpace

namespace Sandbox
{
    void SandboxApplication::onInitialize(Uncarved::ApplicationSpace::RuntimeContext& outRuntimeContext)
    {
        for (const auto& actor : outRuntimeContext.outWorld_.getActors())
        {
            if (actor.getActorName() == "Character")
            {
                characterActorId_ = actor.getId();
                break;
            }
        }

        outFrontSprite_ = outRuntimeContext.outSpriteCatalog_.findSprite("CharacterFront");
        outBackSprite_ = outRuntimeContext.outSpriteCatalog_.findSprite("CharacterBack");
        outHitSprite_ = outRuntimeContext.outSpriteCatalog_.findSprite("CharacterHit");

        bFacingBack_ = false;
        bFacingLeft_ = false;
    }

    void SandboxApplication::onUpdate(Uncarved::ApplicationSpace::RuntimeContext& outRuntimeContext)
    {
        const auto& inputSystem = outRuntimeContext.outInputSystem_;

        const bool bMoveUp = inputSystem.isKeyDown(Uncarved::InputSpace::Key::W)
            || inputSystem.isKeyDown(Uncarved::InputSpace::Key::ArrowUp);

        const bool bMoveDown = inputSystem.isKeyDown(Uncarved::InputSpace::Key::S)
            || inputSystem.isKeyDown(Uncarved::InputSpace::Key::ArrowDown);

        const bool bMoveLeft = inputSystem.isKeyDown(Uncarved::InputSpace::Key::A)
            || inputSystem.isKeyDown(Uncarved::InputSpace::Key::ArrowLeft);

        const bool bMoveRight = inputSystem.isKeyDown(Uncarved::InputSpace::Key::D)
            || inputSystem.isKeyDown(Uncarved::InputSpace::Key::ArrowRight);

        const bool bHit = inputSystem.isKeyDown(Uncarved::InputSpace::Key::Space);

        movementIntent_ = {
            static_cast<float>(bMoveRight) - static_cast<float>(bMoveLeft),
            static_cast<float>(bMoveDown) - static_cast<float>(bMoveUp)
        };

        const bool bIsMoving = glm::dot(movementIntent_, movementIntent_) > 0.0f;

        if (bIsMoving)
        {
            movementIntent_ = glm::normalize(movementIntent_);
        }

        if (movementIntent_.y < 0.0f)
        {
            bFacingBack_ = true;
        }
        else if (movementIntent_.y > 0.0f)
        {
            bFacingBack_ = false;
        }

        if (movementIntent_.x < 0.0f)
        {
            bFacingLeft_ = true;
        }
        else if (movementIntent_.x > 0.0f)
        {
            bFacingLeft_ = false;
        }

        auto& actor = *outRuntimeContext.outWorld_.getActorById(characterActorId_);
        auto& transform2d = actor.getTransform2d();
        auto& runtimeSpriteVisual2d = actor.getRuntimeSpriteVisual2d();

        const float deltaSeconds = static_cast<float>(outRuntimeContext.outAppTime_.getDeltaTime().count());

        transform2d.position_ += movementIntent_ * kMovementSpeed_ * deltaSeconds;

        if (bIsMoving)
        {
            walkingElapsedSeconds_ += deltaSeconds;

            const float bounceOffsetPixels =
                -std::abs(std::sin(walkingElapsedSeconds_ * kBounceAngularFrequency_))
                * kBounceAmplitudePixels_;

            runtimeSpriteVisual2d.setRenderOffsetPixels({0.0f, bounceOffsetPixels});
        }
        else
        {
            walkingElapsedSeconds_ = 0.0f;
            runtimeSpriteVisual2d.setRenderOffsetPixels({0.0f, 0.0f});
        }

        if (bHit)
        {
            runtimeSpriteVisual2d.setSprite(outHitSprite_);
        }
        else
        {
            if (bFacingBack_)
            {
                runtimeSpriteVisual2d.setSprite(outBackSprite_);
            }
            else
            {
                runtimeSpriteVisual2d.setSprite(outFrontSprite_);
            }
        }

        if (bFacingLeft_)
        {
            runtimeSpriteVisual2d.setScale({-1, 1});
        }
        else
        {
            runtimeSpriteVisual2d.setScale({1, 1});
        }
    }

    void SandboxApplication::onShutdown(Uncarved::ApplicationSpace::RuntimeContext&)
    {
    }
} // namespace Sandbox
