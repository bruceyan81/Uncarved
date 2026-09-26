#pragma once

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
            bool        bBlocking,
            float       rotationRadians,
            glm::ivec2  position,
            glm::ivec2  velocity,
            int         zOrder,
            glm::fvec2  scale,
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

        ActorId getId() const noexcept
        {
            return id_;
        }

        bool getBlocking() const noexcept
        {
            return bBlocking_;
        }

        float getRotationRadians() const noexcept
        {
            return rotationRadians_;
        }

        const glm::ivec2& getPosition() const noexcept
        {
            return position_;
        }

        const glm::ivec2& getVelocity() const noexcept
        {
            return velocity_;
        }

        int getZOrder() const noexcept
        {
            return zOrder_;
        }

        const glm::fvec2& getScale() const noexcept
        {
            return scale_;
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

        bool        bBlocking_;
        float       rotationRadians_;
        ActorId     id_;
        glm::ivec2  position_;
        glm::ivec2  velocity_;
        int         zOrder_;
        glm::fvec2  scale_;
        std::string actorName_;

        const ViewSpace::Sprite* outSprite_;

        friend class GameSpace::World;

        void moveBy(const glm::ivec2& delta)
        {
            position_ += delta;
        }

        void setVelocity(const glm::ivec2& newVelocity)
        {
            velocity_ = newVelocity;
        }
    };
} // namespace Uncarved::ObjectSpace
