#pragma once

#include "DataDefinition.h"

#include "Object/DefinitionalActor.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Uncarved::ContentSpace
{
    using GameConfig = std::variant<int, std::string, std::vector<std::string>>;
    using RenderingConfig = std::variant<int>;

    class GameContentLoader final
    {
    public:
        GameContentLoader() = default;

        GameContentLoader(const GameContentLoader&) = delete;
        GameContentLoader& operator=(const GameContentLoader&) = delete;

        GameContentLoader(GameContentLoader&&) noexcept = default;
        GameContentLoader& operator=(GameContentLoader&&) noexcept = default;

        ~GameContentLoader() = default;

        const std::unordered_map<std::string, GameConfig>& getGameConfig() const noexcept
        {
            return gameConfig_;
        }

        const std::unordered_map<std::string, RenderingConfig>& getRenderingConfig() const noexcept
        {
            return renderingConfig_;
        }

        const std::vector<ObjectSpace::DefinitionalActor>& getDefinitionalActors() const noexcept
        {
            return definitionalActors_;
        }

        void releaseLoadData()
        {
            std::vector<Definition::ActorDataPatch>{}.swap(rawActors_);
            std::vector<ObjectSpace::DefinitionalActor>{}.swap(definitionalActors_);
        }

        void checkResourceDirectory() const;

        void checkGameConfig() const;

        bool loadGameConfig();

        bool checkRenderingConfig() const;

        void loadRenderingConfig();

        ContentSpace::Definition::ResourceLoadResult checkSceneResource(std::string_view sceneName) const;

        ContentSpace::Definition::ResourceLoadResult loadSceneResource(std::string_view sceneName);

        bool checkActorTemplate(std::string_view actorName) const;

        Definition::ActorDataPatch loadActorTemplate(std::string_view actorName);

    private:
        std::unordered_map<std::string, GameConfig>      gameConfig_;
        std::unordered_map<std::string, RenderingConfig> renderingConfig_;

        std::vector<Definition::ActorDataPatch>   rawActors_;
        std::vector<ObjectSpace::DefinitionalActor> definitionalActors_;
    };
} // namespace Uncarved::ContentSpace
