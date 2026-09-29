#pragma once

#include "Transform2D.h"

#include "View/Sprite.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <limits>
#include <string>

namespace Uncarved::GameSpace
{
    class World;
} // namespace Uncarved::GameSpace

namespace Uncarved::ObjectSpace
{
    using ActorId = std::uint64_t;

    class Actor
    {
    public:
        Actor(
            Transform2D transform2d,
            int         zOrder,
            std::string actorName,

            const ViewSpace::Sprite* outSprite
        );

        Actor(const Actor&) = delete;
        Actor& operator=(const Actor&) = delete;

        Actor(Actor&& other) noexcept;
        Actor& operator=(Actor&&) noexcept = delete;

        bool operator==(const Actor& other) const noexcept
        {
            return this->id_ == other.id_;
        }

        const Transform2D& getTransform2d() const noexcept
        {
            return transform2d_;
        }

        ActorId getId() const noexcept
        {
            return id_;
        }

        int getZOrder() const noexcept
        {
            return zOrder_;
        }

        const std::string& getActorName() const noexcept
        {
            return actorName_;
        }

        const ViewSpace::Sprite* getSprite() const noexcept
        {
            return outSprite_;
        }

        ~Actor() = default;

    private:
        inline static ActorId    actorCount_ = 0;
        static constexpr ActorId kInvalidId_ = std::numeric_limits<ActorId>::max();

        Transform2D transform2d_{};
        ActorId     id_;
        int         zOrder_;
        std::string actorName_;

        const ViewSpace::Sprite* outSprite_;

        friend class GameSpace::World;
    };
} // namespace Uncarved::ObjectSpace
