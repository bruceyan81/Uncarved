#pragma once

#include "Object/Actor.h"
#include "Object/DefinitionalActor.h"
#include "Time/WorldTime.h"

#include <glm/glm.hpp>

#include <optional>
#include <span>
#include <string>
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

        const ObjectSpace::Actor* getActorById(ObjectSpace::ActorId id) const noexcept;
        ObjectSpace::Actor*       getActorById(ObjectSpace::ActorId id) noexcept;

        std::optional<World> createReplacement(
            std::span<const ObjectSpace::DefinitionalActor>           definitionalActors,
            const std::unordered_map<std::string, ViewSpace::Sprite>& spritesByName
        ) const;

        void updateTime(TimeSpace::Duration deltaTime) noexcept;

        const TimeSpace::WorldTime& getWorldTime() const noexcept
        {
            return worldTime_;
        }

    private:
        std::vector<ObjectSpace::Actor>                       actors_{};
        std::unordered_map<ObjectSpace::ActorId, std::size_t> actorIndexById_{};

        void addActor(const ObjectSpace::DefinitionalActor& definitionalActor, const ViewSpace::Sprite* outSprite);

        TimeSpace::WorldTime worldTime_{};
    };
} // namespace Uncarved::GameSpace
