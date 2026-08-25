#include "Actor.h"

#include <utility>

namespace Uncarved::ObjectSpace
{
    Actor::Actor(
        bool        bBlocking,
        char        view,
        glm::ivec2  position,
        glm::ivec2  velocity,
        std::string actorName,
        std::string nearbyDialogue,
        std::string contactDialogue
    )
        : bBlocking_(bBlocking)
        , view_(view)
        , position_(std::move(position))
        , velocity_(std::move(velocity))
        , actorName_(std::move(actorName))
        , nearbyDialogue_(std::move(nearbyDialogue))
        , contactDialogue_(std::move(contactDialogue))
    {
        id_ = actorCount_++;
    }

    Actor::Actor(Actor&& other) noexcept
        : bBlocking_(other.bBlocking_)
        , bHasUppedScore_(other.bHasUppedScore_)
        , view_(other.view_)
        , id_(std::exchange(other.id_, kInvalidId_))
        , position_(std::move(other.position_))
        , velocity_(std::move(other.velocity_))
        , actorName_(std::move(other.actorName_))
        , nearbyDialogue_(std::move(other.nearbyDialogue_))
        , contactDialogue_(std::move(other.contactDialogue_))
    {
    }

    bool Actor::tryConsumeScoreAward()
    {
        if (!bHasUppedScore_)
        {
            bHasUppedScore_ = true;
            return bHasUppedScore_;
        }
        return false;
    }
} // namespace Uncarved::ObjectSpace
