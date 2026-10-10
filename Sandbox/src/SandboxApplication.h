#pragma once

#include "Application/Application.h"

#include "Object/Actor.h"

#include <glm/glm.hpp>

namespace Sandbox
{
    class SandboxApplication final : public Uncarved::ApplicationSpace::Application
    {
    public:
        void onInitialize(Uncarved::ApplicationSpace::RuntimeContext& outRuntimeContext) override;
        void onUpdate(Uncarved::ApplicationSpace::RuntimeContext& outRuntimeContext) override;
        void onShutdown(Uncarved::ApplicationSpace::RuntimeContext&) override;

    private:
        static constexpr float kMovementSpeed_               = 5.0f;
        static constexpr float kBounceAmplitudePixels_       = 8.0f;
        static constexpr float kBounceAngularFrequency_      = 10.0f;

        Uncarved::ObjectSpace::ActorId characterActorId_{};

        const Uncarved::ViewSpace::Sprite* outFrontSprite_{nullptr};
        const Uncarved::ViewSpace::Sprite* outBackSprite_{nullptr};
        const Uncarved::ViewSpace::Sprite* outHitSprite_{nullptr};

        bool bFacingBack_{};
        bool bFacingLeft_{};

        glm::fvec2 movementIntent_{};
        float      walkingElapsedSeconds_{};
    };
} // namespace Sandbox
