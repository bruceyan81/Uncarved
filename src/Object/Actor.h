#pragma once

#include <glm/glm.hpp>

#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

namespace Uncarved::ObjectSpace
{
    using ActorId = std::uint64_t;

    class Actor
    {
    public:
        Actor(
            bool        bBlocking,
            char        view,
            glm::ivec2  position,
            glm::ivec2  velocity,
            std::string actorName,
            std::string nearbyDialogue,
            std::string contactDialogue
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

        bool getHasUppedScore() const noexcept
        {
            return bHasUppedScore_;
        }

        void updateHasUppedScore(bool bHasUppedScore) noexcept
        {
            bHasUppedScore_ = bHasUppedScore;
        }

        const glm::ivec2& getPosition() const noexcept
        {
            return position_;
        }

        const glm::ivec2& getVelocity() const noexcept
        {
            return velocity_;
        }

        void updateVelocity(const glm::ivec2& newVelocity)
        {
            velocity_ = newVelocity;
        }

        void moveBy(const glm::ivec2& delta)
        {
            position_ += delta;
        }

        const std::string& getActorName() const noexcept
        {
            return actorName_;
        }

        std::string_view getContactDialogue() const noexcept
        {
            return contactDialogue_;
        }

        std::string_view getNearbyDialogue() const noexcept
        {
            return nearbyDialogue_;
        }

        char getActorView() const noexcept
        {
            return view_;
        }

        ~Actor() = default;

    private:
        inline static ActorId    actorCount_ = 0;
        static constexpr ActorId kInvalidId_ = std::numeric_limits<ActorId>::max();

        bool        bBlocking_;
        bool        bHasUppedScore_{false};
        char        view_;
        ActorId     id_;
        glm::ivec2  position_;
        glm::ivec2  velocity_;
        std::string actorName_;
        std::string nearbyDialogue_;
        std::string contactDialogue_;
    };
} // namespace Uncarved::ObjectSpace
