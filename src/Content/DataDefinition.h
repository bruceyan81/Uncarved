#pragma once

#include "Object/Transform2D.h"

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
        MissingTemplate,
        MissingTexture,
        MissingFont
    };

    struct ResourceLoadResult final
    {
        ResourceLoadError error_{ResourceLoadError::None};
        std::string       message_{};

        bool isSucceeded(ResourceLoadError errorType = ResourceLoadError::None) const noexcept
        {
            return error_ == errorType;
        }

        void showErrorMessage() const noexcept
        {
            std::cout << message_;
        }
    };

    struct Transform2DPatch final
    {
        std::optional<float> positionX_{};
        std::optional<float> positionY_{};
        std::optional<float> rotationRadians_{};
        std::optional<float> scaleX_{};
        std::optional<float> scaleY_{};
    };

    struct ActorDataPatch final
    {
        Transform2DPatch           transform2dPatch_;
        std::optional<int>         zOrder_;
        std::optional<std::string> actorName_;
        std::optional<std::string> viewSpriteName_;
    };

    struct GameConfigDefinition final
    {
        std::string gameTitle_{};
        std::string initialSceneName_{};
        std::string fontPath_{};
    };

    struct RenderingConfigDefinition final
    {
        float cameraOrthoWidth_{0.0f};
        int   xResolution_{640};
        int   yResolution_{360};
        int   clearColorR_{0};
        int   clearColorG_{0};
        int   clearColorB_{0};
    };
} // namespace Uncarved::ContentSpace::Definition
