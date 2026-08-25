#pragma once

#include "GameCommon.h"
#include "Gameplay.h"

#include "Object/Actor.h"
#include "Object/DefinitionalActor.h"

#include <glm/glm.hpp>

#include <array>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace Uncarved::GameSpace
{

    struct GameStateRequest
    {
        DialogueCommand intention_;
        std::string     sceneName_;
    };

    class World final
    {
    public:
        World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;

        World(World&&) noexcept = default;
        World& operator=(World&&) noexcept = default;

        ~World() = default;

        const std::vector<ObjectSpace::Actor>& getActors() const noexcept
        {
            return actors_;
        }

        const ObjectSpace::Actor* getPlayer() const noexcept
        {
            if (playerIndex_)
            {
                return &actors_[playerIndex_.value()];
            }
            else
            {
                return nullptr;
            }
        }

        bool moveActorBy(ObjectSpace::ActorId actorId, const glm::ivec2& delta) noexcept;

        void createRequest(DialogueCommand intention, std::string& nextSceneName);

        const std::optional<GameStateRequest>& getGameStateRequest() const noexcept
        {
            return request_;
        }

        const std::array<std::size_t, kMapSize>& getNpcOccupancyGrid() const noexcept
        {
            return npcOccupancyGrid_;
        }

        const std::array<std::size_t, kMapSize>& getBlockingOccupancyGrid() const noexcept
        {
            return blockingOccupancyGrid_;
        }

        ObjectSpace::Actor* getActorById(ObjectSpace::ActorId id)
        {
            const auto it = actorIndexById_.find(id);

            if (it == actorIndexById_.end())
            {
                return nullptr;
            }

            return &actors_[it->second];
        }

        bool tryLoadActors(std::span<const ObjectSpace::DefinitionalActor> definitionalActors);

        void clearWorld() noexcept;

    private:
        std::optional<std::size_t>                            playerIndex_{};
        std::vector<ObjectSpace::Actor>                       actors_{};
        std::unordered_map<ObjectSpace::ActorId, std::size_t> actorIndexById_{};
        std::array<std::size_t, kMapSize>                     npcOccupancyGrid_{};
        std::array<std::size_t, kMapSize>                     blockingOccupancyGrid_{};
        std::optional<GameStateRequest>                       request_{std::nullopt};

        void addActor(const ObjectSpace::DefinitionalActor& definitionalActor);
    };
} // namespace Uncarved::GameSpace
