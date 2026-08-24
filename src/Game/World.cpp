#include "World.h"

#include <optional>
#include <utility>

namespace Uncarved::GameSpace
{
    void World::createRequest(DialogueCommand intention, std::string& nextSceneName)
    {
        if (request_ != std::nullopt)
        {
            return;
        }

        request_ = GameStateRequest{intention, std::move(nextSceneName)};
    }

    bool World::tryLoadActors(std::span<const ObjectSpace::DefinitionalActor> definitionalActors)
    {
        World nextWorld{};

        nextWorld.actors_.reserve(definitionalActors.size());

        for (const auto& definitionalActor : definitionalActors)
        {
            if (definitionalActor.x_ >= 0 && definitionalActor.x_ < kMapWidth && definitionalActor.y_ >= 0
                && definitionalActor.y_ < kMapHeight)
            {
                continue;
            }
            else
            {
                return false;
            }
        }

        for (const auto& definitionalActor : definitionalActors)
        {
            nextWorld.addActor(definitionalActor);
        }

        auto* playerPtr = nextWorld.getPlayer();

        for (const auto& actor : nextWorld.actors_)
        {
            const glm::ivec2 position = actor.getPosition();

            if (!playerPtr || actor.getId() != playerPtr->getId())
            {
                nextWorld.npcOccupancyGrid_[position.y * kMapWidth + position.x]++;
            }

            if (actor.getBlocking())
            {
                nextWorld.blockingOccupancyGrid_[position.y * kMapWidth + position.x]++;
            }
        }

        *this = std::move(nextWorld);

        return true;
    }

    void World::clearWorld() noexcept
    {
        actors_.clear();
        actorIndexById_.clear();
        playerIndex_.reset();
        npcOccupancyGrid_.fill(0);
        blockingOccupancyGrid_.fill(0);
        request_.reset();
    }

    void World::addActor(const ObjectSpace::DefinitionalActor& definitionalActor)
    {
        const char view = definitionalActor.view_.empty() ? '?' : definitionalActor.view_.front();

        actors_.emplace_back(
            definitionalActor.bBlocking_,
            view,
            glm::ivec2{definitionalActor.x_, definitionalActor.y_},
            glm::ivec2{definitionalActor.velX_, definitionalActor.velY_},
            definitionalActor.actorName_,
            definitionalActor.nearbyDialogue_,
            definitionalActor.contactDialogue_
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
