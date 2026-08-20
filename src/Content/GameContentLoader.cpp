#include "GameContentLoader.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace Uncarved
{
    namespace
    {
        ContentSpace::Definition::ActorDataPatch makeActorDataPatch(const rapidjson::Value& rawActor)
        {
            ContentSpace::Definition::ActorDataPatch patch{};

            if (const auto it = rawActor.FindMember("blocking"); it != rawActor.MemberEnd() && it->value.IsBool())
            {
                patch.bBlocking_ = it->value.GetBool();
            }

            if (const auto it = rawActor.FindMember("x"); it != rawActor.MemberEnd() && it->value.IsInt())
            {
                patch.x_ = it->value.GetInt();
            }

            if (const auto it = rawActor.FindMember("y"); it != rawActor.MemberEnd() && it->value.IsInt())
            {
                patch.y_ = it->value.GetInt();
            }

            if (const auto it = rawActor.FindMember("vel_x"); it != rawActor.MemberEnd() && it->value.IsInt())
            {
                patch.velX_ = it->value.GetInt();
            }

            if (const auto it = rawActor.FindMember("vel_y"); it != rawActor.MemberEnd() && it->value.IsInt())
            {
                patch.velY_ = it->value.GetInt();
            }

            if (const auto it = rawActor.FindMember("view"); it != rawActor.MemberEnd() && it->value.IsString())
            {
                patch.view_ = it->value.GetString();
            }

            if (const auto it = rawActor.FindMember("name"); it != rawActor.MemberEnd() && it->value.IsString())
            {
                patch.actorName_ = it->value.GetString();
            }

            if (const auto it = rawActor.FindMember("nearby_dialogue");
                it != rawActor.MemberEnd() && it->value.IsString())
            {
                patch.nearbyDialogue_ = it->value.GetString();
            }

            if (const auto it = rawActor.FindMember("contact_dialogue");
                it != rawActor.MemberEnd() && it->value.IsString())
            {
                patch.contactDialogue_ = it->value.GetString();
            }

            return patch;
        }

        void applyActorDataPatch(
            ObjectSpace::ActorDefinition&                   outDefinition,
            const ContentSpace::Definition::ActorDataPatch& outPatch
        )
        {
            if (outPatch.bBlocking_)
            {
                outDefinition.bBlocking_ = *outPatch.bBlocking_;
            }

            if (outPatch.x_)
            {
                outDefinition.x_ = *outPatch.x_;
            }

            if (outPatch.y_)
            {
                outDefinition.y_ = *outPatch.y_;
            }

            if (outPatch.velX_)
            {
                outDefinition.velX_ = *outPatch.velX_;
            }

            if (outPatch.velY_)
            {
                outDefinition.velY_ = *outPatch.velY_;
            }

            if (outPatch.view_)
            {
                outDefinition.view_ = *outPatch.view_;
            }

            if (outPatch.actorName_)
            {
                outDefinition.actorName_ = *outPatch.actorName_;
            }

            if (outPatch.nearbyDialogue_)
            {
                outDefinition.nearbyDialogue_ = *outPatch.nearbyDialogue_;
            }

            if (outPatch.contactDialogue_)
            {
                outDefinition.contactDialogue_ = *outPatch.contactDialogue_;
            }
        }

        ObjectSpace::ActorDefinition resolveActorDefinition(const ContentSpace::Definition::ActorDataPatch& patch)
        {
            ObjectSpace::ActorDefinition definition{};
            applyActorDataPatch(definition, patch);
            return definition;
        }

        ObjectSpace::ActorDefinition resolveActorDefinition(
            const ContentSpace::Definition::ActorDataPatch& templatePatch,
            const ContentSpace::Definition::ActorDataPatch& patch
        )
        {
            ObjectSpace::ActorDefinition definition{};

            applyActorDataPatch(definition, templatePatch);
            applyActorDataPatch(definition, patch);

            return definition;
        }
    } // namespace
} // namespace Uncarved

namespace Uncarved::ContentSpace
{
    namespace Fs = std::filesystem;
    const Fs::path gResourceRoot = "Resources";

    constexpr std::string_view kActorTemplatesDir = "ActorTemplates";
    constexpr std::string_view kScenesDir = "Scenes";

    constexpr std::string_view kGameConfigFileName = "Game";
    constexpr std::string_view kRenderingConfigFileName = "Rendering";

    constexpr std::string_view kConfigFilePostfix = ".config";
    constexpr std::string_view kScenePostfix = ".scene";
    constexpr std::string_view kTemplatePostfix = ".template";

    void GameContentLoader::checkResourceDirectory() const
    {
        if (!Fs::exists(gResourceRoot) || !Fs::is_directory(gResourceRoot))
        {
            std::cout << "error: Resources/ missing.";
            std::exit(EXIT_FAILURE);
        }
    }

    void GameContentLoader::checkGameConfig() const
    {
        const Fs::path gameConfigPath = (gResourceRoot / kGameConfigFileName).replace_extension(kConfigFilePostfix);
        if (!Fs::exists(gameConfigPath))
        {
            std::cout << "error: Resources/Game.config missing.";
            std::exit(EXIT_FAILURE);
        }
    }

    bool GameContentLoader::loadGameConfig()
    {
        std::ifstream inputFileStream{(gResourceRoot / kGameConfigFileName).replace_extension(kConfigFilePostfix)};

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError() || !document.IsObject())
        {
            return false;
        }

        if (document.HasMember("initial_scene") && document["initial_scene"].IsString())
        {
            gameConfig_.emplace("initial_scene", document["initial_scene"].GetString());
        }
        else
        {
            std::cout << "error: initial_scene unspecified.";
            std::exit(EXIT_FAILURE);
        }

        if (document.HasMember("game_title") && document["game_title"].IsString())
        {
            gameConfig_.emplace("game_title", document["game_title"].GetString());
        }

        if (document.HasMember("health") && document["health"].IsInt())
        {
            gameConfig_.emplace("health", document["health"].GetInt());
        }

        if (document.HasMember("score") && document["score"].IsInt())
        {
            gameConfig_.emplace("score", document["score"].GetInt());
        }

        if (document.HasMember("game_start_message") && document["game_start_message"].IsString())
        {
            gameConfig_.emplace("game_start_message", document["game_start_message"].GetString());
        }

        if (document.HasMember("game_over_bad_message") && document["game_over_bad_message"].IsString())
        {
            gameConfig_.emplace("game_over_bad_message", document["game_over_bad_message"].GetString());
        }

        if (document.HasMember("game_over_good_message") && document["game_over_good_message"].IsString())
        {
            gameConfig_.emplace("game_over_good_message", document["game_over_good_message"].GetString());
        }

        return true;
    }

    bool GameContentLoader::checkRenderingConfig() const
    {
        const Fs::path renderingConfigPath =
            (gResourceRoot / kRenderingConfigFileName).replace_extension(kConfigFilePostfix);

        if (!Fs::exists(renderingConfigPath))
        {
            std::cout << "info: Resources/Rendering.config missing.";
            return false;
        }

        return true;
    }

    void GameContentLoader::loadRenderingConfig()
    {
        std::ifstream inputFileStream{(gResourceRoot / kRenderingConfigFileName).replace_extension(kConfigFilePostfix)};

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            std::cout << "info: Resources/Rendering.config has parse error.";
            return;
        }

        if (!document.IsObject())
        {
            std::cout << "info: Resources/Rendering.config is not Object.";
            return;
        }

        if (document.HasMember("x_resolution") && document["x_resolution"].IsInt())
        {
            renderingConfig_.emplace("x_resolution", document["x_resolution"].GetInt());
        }
        else
        {
            std::cout << "info: Resources/Rendering.config has not x_resolution.";
        }

        if (document.HasMember("y_resolution") && document["y_resolution"].IsInt())
        {
            renderingConfig_.emplace("y_resolution", document["y_resolution"].GetInt());
        }
        else
        {
            std::cout << "info: Resources/Rendering.config has not y_resolution.";
        }

        if (document.HasMember("clear_color_r") && document["clear_color_r"].IsInt())
        {
            renderingConfig_.emplace("clear_color_r", document["clear_color_r"].GetInt());
        }

        if (document.HasMember("clear_color_g") && document["clear_color_g"].IsInt())
        {
            renderingConfig_.emplace("clear_color_g", document["clear_color_g"].GetInt());
        }

        if (document.HasMember("clear_color_b") && document["clear_color_b"].IsInt())
        {
            renderingConfig_.emplace("clear_color_b", document["clear_color_b"].GetInt());
        }
    }

    ContentSpace::Definition::ResourceLoadResult GameContentLoader::checkSceneResource(std::string_view sceneName) const
    {
        Fs::path scenePath = (gResourceRoot / kScenesDir / std::string{sceneName}).replace_extension(kScenePostfix);

        if (!Fs::exists(scenePath) || !Fs::is_regular_file(scenePath))
        {
            return {
                ContentSpace::Definition::ResourceLoadError::NotFound,
                std::format("error: scene {} is missing.", sceneName)
            };
        }

        return {};
    }

    ContentSpace::Definition::ResourceLoadResult GameContentLoader::loadSceneResource(std::string_view sceneName)
    {
        std::ifstream inputFileStream{
            (gResourceRoot / kScenesDir / std::string{sceneName}).replace_extension(kScenePostfix)
        };

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {
                ContentSpace::Definition::ResourceLoadError::ParseFailed,
                std::format("info: Resources/Scenes/{}.scene has parse error.", sceneName)
            };
        }

        if (!document.IsObject())
        {
            return {
                ContentSpace::Definition::ResourceLoadError::InvalidStructure,
                std::format("info: Resources/Scenes/{}.scene is not Object.", sceneName)
            };
        }

        if (!document.HasMember("actors") || !document["actors"].IsArray())
        {
            return {
                ContentSpace::Definition::ResourceLoadError::InvalidActor,
                std::format("info: Resources/Scenes/{}.scene has invalid actors.", sceneName)
            };
        }

        const auto& actors = document["actors"];

        std::vector<Definition::ActorDataPatch>   tempRawActors;
        std::vector<ObjectSpace::ActorDefinition> tempDefinitionalActors;

        tempRawActors.reserve(actors.Size());
        tempDefinitionalActors.reserve(actors.Size());

        for (const auto& rawActor : actors.GetArray())
        {
            if (!rawActor.IsObject())
            {
                continue;
            }

            tempRawActors.emplace_back(makeActorDataPatch(rawActor));

            const auto templateIt = rawActor.FindMember("template");

            if (templateIt == rawActor.MemberEnd())
            {
                tempDefinitionalActors.emplace_back(resolveActorDefinition(tempRawActors.back()));

                continue;
            }

            if (!templateIt->value.IsString())
            {
                return {Definition::ResourceLoadError::InvalidActor, "info: actor template property is not String."};
            }

            const std::string_view templateName = templateIt->value.GetString();

            if (!checkActorTemplate(templateName))
            {
                return {
                    Definition::ResourceLoadError::MissingTemplate,
                    std::format("error: template {} is missing.", templateName)
                };
            }

            auto templatePatch = loadActorTemplate(templateName);

            tempDefinitionalActors.emplace_back(resolveActorDefinition(templatePatch, tempRawActors.back()));
        }

        rawActors_.swap(tempRawActors);
        definitionalActors_.swap(tempDefinitionalActors);

        return {};
    }

    bool GameContentLoader::checkActorTemplate(std::string_view actorName) const
    {
        const Fs::path templatePath =
            (gResourceRoot / kActorTemplatesDir / actorName).replace_extension(kTemplatePostfix);

        if (!Fs::exists(templatePath))
        {
            return false;
        }

        return true;
    }

    Definition::ActorDataPatch GameContentLoader::loadActorTemplate(std::string_view actorName)
    {
        std::ifstream inputFileStream{
            (gResourceRoot / kActorTemplatesDir / actorName).replace_extension(kTemplatePostfix)
        };

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError() || !document.IsObject())
        {
            std::cout << "error: Resources/ActorTemplates/" << actorName << kTemplatePostfix << " is not Object.";
            std::exit(EXIT_FAILURE);
        }

        return makeActorDataPatch(document);
    }
} // namespace Uncarved::ContentSpace
