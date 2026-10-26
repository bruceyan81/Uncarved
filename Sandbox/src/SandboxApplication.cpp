#include "SandboxApplication.h"

#include "Application/RuntimeContext.h"
#include "Collision/CollisionQuery.h"
#include "Game/World.h"
#include "Input/InputSystem.h"
#include "Input/Key.h"
#include "Logging/Log.h"
#include "Object/Actor.h"
#include "Object/Transform2D.h"
#include "Time/AppTime.h"
#include "View/SpriteCatalog.h"

#include <glm/geometric.hpp>

#include <cmath>
#include <memory>
#include <vector>

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
        auto&       world = outRuntimeContext.outWorld_;
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

        auto* actorPtr = world.getActorById(characterActorId_);

        if (actorPtr == nullptr)
        {
            UC_LOG(
                Uncarved::gRuntime,
                Uncarved::LogVerbosity::Error,
                "Actor is not find. Actor id: {}",
                characterActorId_
            );
            return;
        }

        auto& actor = *actorPtr;
        auto& runtimeSpriteVisual2d = actor.getRuntimeSpriteVisual2d();

        const float      deltaSeconds = static_cast<float>(outRuntimeContext.outAppTime_.getDeltaTime().count());
        const glm::fvec2 movementDelta = movementIntent_ * kMovementSpeed_ * deltaSeconds;

        const Uncarved::ObjectSpace::Transform2D& currentTransform2d = actor.getTransform2d();

        Uncarved::ObjectSpace::Transform2D  candidateTransform2d = currentTransform2d;
        candidateTransform2d.position_ += movementDelta;

        bool        bMovementBlocked = false;
        const auto& actorShapes = actor.getCollisionShapes();

        std::vector<Uncarved::CollisionSpace::CollisionQueryResult> queryResults{};

        for (const auto& shape : actorShapes)
        {
            if (shape.getMode() != Uncarved::CollisionSpace::CollisionMode::Solid)
            {
                continue;
            }

            const auto candidateWorldBound = shape.calculateCandidateWorldBound(candidateTransform2d);

            if (!candidateWorldBound.has_value())
            {
                UC_LOG(
                    Uncarved::gRuntime,
                    Uncarved::LogVerbosity::Error,
                    "Failed to calculate a solid collision shape candidate world bound."
                );
                return;
            }

            const auto query = Uncarved::CollisionSpace::CollisionQuery::createCollisionQuery(
                *candidateWorldBound,
                shape.getFilter(),
                actor.getId()
            );

            if (!query.has_value())
            {
                UC_LOG(Uncarved::gRuntime, Uncarved::LogVerbosity::Error, "Failed to create a solid collision query.");
                return;
            }

            queryResults = world.queryOverlaps(*query);

            for (const auto& result : queryResults)
            {
                if (result.mode_ == Uncarved::CollisionSpace::CollisionMode::Solid)
                {
                    bMovementBlocked = true;
                    UC_LOG(Uncarved::gRuntime, Uncarved::LogVerbosity::Info, "Collision info: Has a collision.");
                    break;
                }
            }

            if (bMovementBlocked)
            {
                break;
            }
        }

        if (!bMovementBlocked)
        {
            if (!world.updateActorTransform2dById(actor.getId(), candidateTransform2d))
            {
                UC_LOG(Uncarved::gRuntime, Uncarved::LogVerbosity::Error, "Failed to update the actor transform.");
                return;
            }
        }

        for (const auto& shape : actorShapes)
        {
            if (shape.getMode() != Uncarved::CollisionSpace::CollisionMode::Sensor)
            {
                continue;
            }

            const auto query = Uncarved::CollisionSpace::CollisionQuery::createCollisionQuery(
                shape.getWorldBound(),
                shape.getFilter(),
                actor.getId()
            );

            if (!query.has_value())
            {
                UC_LOG(Uncarved::gRuntime, Uncarved::LogVerbosity::Error, "Failed to create a sensor collision query.");
                return;
            }

            queryResults = world.queryOverlaps(*query);

            for (const auto& result : queryResults)
            {
                if (result.mode_ == Uncarved::CollisionSpace::CollisionMode::Sensor)
                {
                    UC_LOG(Uncarved::gRuntime, Uncarved::LogVerbosity::Info, "Collision info: Sensor is triggered.");
                    break;
                }
            }
        }

        if (bIsMoving)
        {
            walkingElapsedSeconds_ += deltaSeconds;

            const float bounceOffsetPixels =
                -std::abs(std::sin(walkingElapsedSeconds_ * kBounceAngularFrequency_)) * kBounceAmplitudePixels_;

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
