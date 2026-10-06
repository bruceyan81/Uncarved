#pragma once

#include "Object/Transform2D.h"

#include <optional>
#include <string>

namespace Uncarved::ContentSpace::Definition
{
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
