#pragma once

#include "Object/Actor.h"
#include "Object/DefinitionalActor.h"

#include <glm/glm.hpp>

#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace Uncarved::GameSpace
{
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

        bool setActorVelocity(ObjectSpace::ActorId actorId, const glm::ivec2& velocity) noexcept;

        const ObjectSpace::Actor* getActorById(ObjectSpace::ActorId id) const noexcept;

        void loadActors(std::span<const ObjectSpace::DefinitionalActor> definitionalActors);

        void clearWorld() noexcept;

    private:
        std::optional<std::size_t>                            playerIndex_{};
        std::vector<ObjectSpace::Actor>                       actors_{};
        std::unordered_map<ObjectSpace::ActorId, std::size_t> actorIndexById_{};

        void addActor(const ObjectSpace::DefinitionalActor& definitionalActor);
    };
} // namespace Uncarved::GameSpace
