#pragma once

#include "Collision/CollisionWorld.h"
#include "Object/Actor.h"
#include "Object/DefinitionalActor.h"
#include "Object/ObjectTypes.h"
#include "Time/WorldTime.h"

#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace Uncarved::CollisionSpace
{
    struct CollisionQuery;
}

namespace Uncarved::ObjectSpace
{
    struct Transform2D;
}

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
        );

        void updateTime(TimeSpace::Duration deltaTime) noexcept;

        const TimeSpace::WorldTime& getWorldTime() const noexcept
        {
            return worldTime_;
        }

        bool updateActorTransform2dById(ObjectSpace::ActorId actorId, const ObjectSpace::Transform2D& candidateTransform2d);

        std::vector<CollisionSpace::CollisionQueryResult> queryOverlaps(
            const CollisionSpace::CollisionQuery& query
        ) const;

    private:
        std::vector<ObjectSpace::Actor>                       actors_{};
        std::unordered_map<ObjectSpace::ActorId, std::size_t> actorIndexById_{};
        CollisionSpace::CollisionWorld                        collisionWorld_{};

        bool addActor(const ObjectSpace::DefinitionalActor& definitionalActor, const ViewSpace::Sprite* outSprite);
        bool registerShapes();

        TimeSpace::WorldTime worldTime_{};
    };
} // namespace Uncarved::GameSpace
