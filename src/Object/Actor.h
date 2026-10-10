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
            ViewSpace::RuntimeSpriteVisual2D runtimeSpriteVisual2d
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

        Transform2D& getTransform2d() noexcept
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

        const ViewSpace::RuntimeSpriteVisual2D& getRuntimeSpriteVisual2d() const noexcept
        {
            return runtimeSpriteVisual2d_;
        }

        ViewSpace::RuntimeSpriteVisual2D& getRuntimeSpriteVisual2d() noexcept
        {
            return runtimeSpriteVisual2d_;
        }

        ~Actor() = default;

    private:
        inline static ActorId    actorCount_ = 0;
        static constexpr ActorId kInvalidId_ = std::numeric_limits<ActorId>::max();

        Transform2D                      transform2d_{};
        ActorId                          id_;
        int                              zOrder_;
        std::string                      actorName_;
        ViewSpace::RuntimeSpriteVisual2D runtimeSpriteVisual2d_;

        friend class GameSpace::World;
    };
} // namespace Uncarved::ObjectSpace
