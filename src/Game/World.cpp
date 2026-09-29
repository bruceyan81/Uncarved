#include "World.h"

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

    void World::updateTime(TimeSpace::Duration deltaTime) noexcept
    {
        worldTime_.updateTime(deltaTime);
    }

    void World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor, const ViewSpace::Sprite* outSprite)
    {
        actors_.emplace_back(
            definitionalActor.transform2d_,
            definitionalActor.zOrder_,
            definitionalActor.actorName_,
            outSprite
        );

        const std::size_t actorIndex = actors_.size() - 1;
        const auto&       actor = actors_.back();

        actorIndexById_[actor.getId()] = actorIndex;
    }
} // namespace Uncarved::GameSpace
