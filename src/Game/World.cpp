#include "World.h"

#include <utility>

namespace Uncarved::GameSpace
{
    bool World::moveActorBy(ObjectSpace::ActorId actorId, const glm::ivec2& delta) noexcept
    {
        auto actorIt = actorIndexById_.find(actorId);

        if (actorIt == actorIndexById_.end())
        {
            return false;
        }

        auto& actor = actors_[actorIt->second];

        actor.moveBy(delta);

        return true;
    }

    bool World::setActorVelocity(ObjectSpace::ActorId actorId, const glm::ivec2& velocity) noexcept
    {

        auto actorIt = actorIndexById_.find(actorId);

        if (actorIt == actorIndexById_.end())
        {
            return false;
        }

        auto& actor = actors_[actorIt->second];

        actor.setVelocity(velocity);

        return true;
    }

    const ObjectSpace::Actor* World::getActorById(ObjectSpace::ActorId id) const noexcept
    {
        const auto it = actorIndexById_.find(id);

        if (it == actorIndexById_.end())
        {
            return nullptr;
        }

        return &actors_[it->second];
    }

    void World::loadActors(std::span<const ObjectSpace::DefinitionalActor> definitionalActors)
    {
        World nextWorld{};

        nextWorld.actors_.reserve(definitionalActors.size());

        for (const auto& definitionalActor : definitionalActors)
        {
            nextWorld.addActor(definitionalActor);
        }

        nextWorld.worldTime_ = worldTime_;

        *this = std::move(nextWorld);
    }

    void World::clearWorld() noexcept
    {
        actors_.clear();
        actorIndexById_.clear();
        playerIndex_.reset();
    }

    void World::updateTime(TimeSpace::Duration deltaTime) noexcept
    {
        worldTime_.updateTime(deltaTime);
    }

    void World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor)
    {
        std::optional<glm::fvec2> normalizedPivot = std::nullopt;
        if (definitionalActor.normalizedPivotX_.has_value() && definitionalActor.normalizedPivotY_.has_value())
        {
            normalizedPivot = {*(definitionalActor.normalizedPivotX_), *(definitionalActor.normalizedPivotY_)};
        }

        std::optional<std::string> viewTextureName = std::nullopt;
        if (definitionalActor.viewTextureName_.has_value())
        {
            viewTextureName = definitionalActor.viewTextureName_;
        }

        actors_.emplace_back(
            definitionalActor.bBlocking_,
            definitionalActor.rotationRadians_,
            glm::ivec2{definitionalActor.x_, definitionalActor.y_},
            glm::ivec2{definitionalActor.velX_, definitionalActor.velY_},
            glm::fvec2{definitionalActor.scaleX_, definitionalActor.scaleY_},
            definitionalActor.actorName_,
            normalizedPivot,
            viewTextureName
        );

        const std::size_t actorIndex = actors_.size() - 1;
        const auto&       actor = actors_.back();

        actorIndexById_[actor.getId()] = actorIndex;

        if (actor.getActorName() == "player")
        {
            playerIndex_ = actorIndex;
        }
    }
} // namespace Uncarved::GameSpace
