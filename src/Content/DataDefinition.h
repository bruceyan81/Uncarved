#pragma once

#include <iostream>
#include <optional>
#include <string>

namespace Uncarved::ContentSpace::Definition
{
    enum class ResourceLoadError
    {
        None,
        NotFound,
        ParseFailed,
        InvalidStructure,
        InvalidActor,
        MissingPlayer,
        MissingTemplate
    };

    struct ResourceLoadResult final
    {
        ResourceLoadError error_{ResourceLoadError::None};
        std::string       message_{};

        bool isSucceeded(ResourceLoadError errorType = ResourceLoadError::None) const noexcept
        {
            return error_ == errorType;
        }

        void showMessage() const noexcept
        {
            std::cout << message_;
        }
    };

    struct ActorDataPatch final
    {
        std::optional<bool>        bBlocking_;
        std::optional<int>         x_;
        std::optional<int>         y_;
        std::optional<int>         velX_;
        std::optional<int>         velY_;
        std::optional<std::string> view_;
        std::optional<std::string> actorName_;
        std::optional<std::string> nearbyDialogue_;
        std::optional<std::string> contactDialogue_;
    };
} // namespace Uncarved::ContentSpace::Definition
