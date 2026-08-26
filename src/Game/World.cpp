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

        *this = std::move(nextWorld);
    }

    void World::clearWorld() noexcept
    {
        actors_.clear();
        actorIndexById_.clear();
        playerIndex_.reset();
    }

    void World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor)
    {
        const char view = definitionalActor.view_.empty() ? '?' : definitionalActor.view_.front();

        actors_.emplace_back(
            definitionalActor.bBlocking_,
            view,
            glm::ivec2{definitionalActor.x_, definitionalActor.y_},
            glm::ivec2{definitionalActor.velX_, definitionalActor.velY_},
            definitionalActor.actorName_
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
