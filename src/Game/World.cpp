#include "World.h"

#include <cstddef>
#include <utility>

namespace Uncarved::ViewSpace
{
    class Sprite;
}

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

    std::optional<World> World::createReplacement(
        std::span<const ObjectSpace::DefinitionalActor>           definitionalActors,
        const std::unordered_map<std::string, ViewSpace::Sprite>& sprites
    ) const
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

            nextWorld.addActor(definitionalActor, sprite);
        }

        nextWorld.worldTime_ = worldTime_;

        return nextWorld;
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

    void World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor, const ViewSpace::Sprite* outSprite)
    {
        actors_.emplace_back(
            definitionalActor.bBlocking_,
            definitionalActor.rotationRadians_,
            glm::ivec2{definitionalActor.x_, definitionalActor.y_},
            glm::ivec2{definitionalActor.velX_, definitionalActor.velY_},
            definitionalActor.zOrder_,
            glm::fvec2{definitionalActor.scaleX_, definitionalActor.scaleY_},
            definitionalActor.actorName_,
            outSprite
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
