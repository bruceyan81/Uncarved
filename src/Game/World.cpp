#include "World.h"

#include "Collision/CollisionQuery.h"
#include "Collision/CollisionShape.h"
#include "Collision/CollisionTypes.h"
#include "Logging/Log.h"
#include "Object/Transform2D.h"
#include "View/Sprite.h"

#include <cstddef>
#include <utility>

namespace Uncarved::ViewSpace
{
    class Sprite;
}

namespace Uncarved::GameSpace
{
    const ObjectSpace::Actor* World::getActorById(ObjectSpace::ActorId id) const noexcept
    {
        const auto it = actorIndexById_.find(id);

        if (it == actorIndexById_.end())
        {
            return nullptr;
        }

        return &actors_[it->second];
    }

    ObjectSpace::Actor* World::getActorById(ObjectSpace::ActorId id) noexcept
    {
        const auto it = actorIndexById_.find(id);

        if (it == actorIndexById_.end())
        {
            return nullptr;
        }

        return &actors_[it->second];
    }

    std::optional<World> World::createReplacement(
        std::span<const ObjectSpace::DefinitionalActor>           definitionalActors,
        const std::unordered_map<std::string, ViewSpace::Sprite>& sprites
    )
    {
        World nextWorld{};

        nextWorld.actors_.reserve(definitionalActors.size());

        for (const auto& definitionalActor : definitionalActors)
        {
            const ViewSpace::Sprite* sprite = nullptr;

            if (definitionalActor.viewSpriteName_)
            {
                const auto spriteIt = sprites.find(*definitionalActor.viewSpriteName_);

                if (spriteIt == sprites.end())
                {
                    return std::nullopt;
                }

                sprite = &spriteIt->second;
            }

            if (!nextWorld.addActor(definitionalActor, sprite))
            {
                return std::nullopt;
            }
        }

        if (!nextWorld.registerShapes())
        {
            return std::nullopt;
        }

        nextWorld.worldTime_ = worldTime_;

        return nextWorld;
    }

    void World::updateTime(TimeSpace::Duration deltaTime) noexcept
    {
        worldTime_.updateTime(deltaTime);
    }

    bool World::updateActorTransform2dById(
        ObjectSpace::ActorId            actorId,
        const ObjectSpace::Transform2D& candidateTransform2d
    )
    {
        auto* actor = getActorById(actorId);

        if (actor == nullptr || !ObjectSpace::isValidTransform2D(candidateTransform2d))
        {
            return false;
        }

        auto& collisionShapes = actor->collisionShapes_;

        if (collisionShapes.empty())
        {
            actor->transform2d_ = candidateTransform2d;
            return true;
        }

        std::vector<CollisionSpace::AxisAlignedBox> candidateWorldBounds{};
        candidateWorldBounds.reserve(collisionShapes.size());

        for (const auto& shape : collisionShapes)
        {
            const auto candidateWorldBound =
                shape.calculateCandidateWorldBound(candidateTransform2d.position_, candidateTransform2d.scale_);

            if (!candidateWorldBound.has_value())
            {
                return false;
            }

            if (!collisionWorld_.hasShape(shape.getCollisionShapeId()))
            {
                return false;
            }

            candidateWorldBounds.push_back(*candidateWorldBound);
        }

        for (std::size_t index = 0; index < collisionShapes.size(); ++index)
        {
            auto&       shape = collisionShapes[index];
            const auto& candidateWorldBound = candidateWorldBounds[index];

            if (!collisionWorld_.updateShapeWorldBound(shape.getCollisionShapeId(), candidateWorldBound))
            {
                return false;
            }

            shape.commitWorldBound(candidateWorldBound);
        }

        actor->transform2d_ = candidateTransform2d;
        return true;
    }

    std::vector<CollisionSpace::CollisionQueryResult>
    GameSpace::World::queryOverlaps(const CollisionSpace::CollisionQuery& query) const
    {
        return collisionWorld_.queryOverlaps(query);
    }

    bool World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor, const ViewSpace::Sprite* outSprite)
    {
        if (!ObjectSpace::isValidTransform2D(definitionalActor.transform2d_))
        {
            return false;
        }

        actors_.emplace_back(
            definitionalActor.transform2d_,
            definitionalActor.zOrder_,
            definitionalActor.actorName_,
            ViewSpace::RuntimeSpriteVisual2D{outSprite}
        );

        auto& actor = actors_.back();
        actor.collisionShapes_.reserve(definitionalActor.collisionShapes_.size());

        for (const auto& definitionalCollisionShape : definitionalActor.collisionShapes_)
        {
            auto collisionShape = CollisionSpace::CollisionShape::createCollisionShape(
                actor.getId(),
                definitionalCollisionShape.localCenter_,
                definitionalCollisionShape.size_,
                actor.getTransform2d().position_,
                actor.getTransform2d().scale_,
                definitionalCollisionShape.filter_,
                CollisionSpace::CollisionShapeType::AxisAlignedBox,
                definitionalCollisionShape.mode_
            );

            if (!collisionShape.has_value())
            {
                actors_.pop_back();
                return false;
            }

            actor.addCollisionShape(std::move(*collisionShape));
        }

        const std::size_t actorIndex = actors_.size() - 1;
        actorIndexById_[actor.getId()] = actorIndex;

        return true;
    }

    bool GameSpace::World::registerShapes()
    {
        collisionWorld_.proxiesByShapeId_.reserve(actors_.size() * 2);

        for (const auto& actor : actors_)
        {
            for (const auto& collisionShape : actor.getCollisionShapes())
            {
                if (!collisionWorld_.registerShape(collisionShape))
                {
                    UC_LOG(
                        gRuntime,
                        LogVerbosity::Trace,
                        "The Shape with id {} failed to register.",
                        collisionShape.getCollisionShapeId()
                    );
                    return false;
                }
            }
        }

        return true;
    }
} // namespace Uncarved::GameSpace
