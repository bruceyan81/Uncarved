#pragma once

#include "ContentResult.h"
#include "DataDefinition.h"

#include "Object/DefinitionalActor.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Uncarved::ContentSpace
{
    class GameContentLoader final
    {
    public:
        GameContentLoader() = default;

        GameContentLoader(const GameContentLoader&) = delete;
        GameContentLoader& operator=(const GameContentLoader&) = delete;

        GameContentLoader(GameContentLoader&&) noexcept = default;
        GameContentLoader& operator=(GameContentLoader&&) noexcept = default;

        ~GameContentLoader() = default;

        ContentResult checkResourceDirectory() const;

        ContentResult checkGameConfig() const;

        ContentResult loadGameConfig();

        ContentResult checkRenderingConfig() const;

        ContentResult loadRenderingConfig();

        ContentResult checkIntroConfig() const;

        ContentResult loadIntroConfig();

        ContentResult checkActorTemplate(std::string_view actorName) const;

        ContentResult loadActorTemplate(std::string_view actorName);

        ContentResult checkSceneResource(std::string_view sceneName) const;

        ContentResult loadSceneResource(std::string_view sceneName);

        void releaseLoadData()
        {
            std::vector<Definition::ActorDataPatch>{}.swap(rawActors_);
            std::vector<ObjectSpace::DefinitionalActor>{}.swap(definitionalActors_);
            std::unordered_map<std::string, Definition::ActorDataPatch>{}.swap(actorTemplatePatchesByName_);
        }

        const Definition::GameConfigDefinition& getGameConfig() const noexcept
        {
            return gameConfigDefinition_;
        }

        const Definition::RenderingConfigDefinition& getRenderingConfig() const noexcept
        {
            return renderingConfigDefinition_;
        }

        const Definition::IntroConfigDefinition& getIntroConfig() const noexcept
        {
            return introConfigDefinition_;
        }

        const std::vector<ObjectSpace::DefinitionalActor>& getDefinitionalActors() const noexcept
        {
            return definitionalActors_;
        }

    private:
        Definition::GameConfigDefinition      gameConfigDefinition_;
        Definition::RenderingConfigDefinition renderingConfigDefinition_;
        Definition::IntroConfigDefinition     introConfigDefinition_;

        std::vector<Definition::ActorDataPatch>     rawActors_;
        std::vector<ObjectSpace::DefinitionalActor> definitionalActors_;

        std::unordered_map<std::string, Definition::ActorDataPatch> actorTemplatePatchesByName_;
    };
} // namespace Uncarved::ContentSpace
