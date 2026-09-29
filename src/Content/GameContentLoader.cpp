#include "GameContentLoader.h"

#include "ContentResult.h"

#include "Object/Transform2D.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <utility>

namespace Uncarved
{
    namespace
    {
        ContentSpace::Definition::ActorDataPatch makeActorDataPatch(const rapidjson::Value& rawActor)
        {
            ContentSpace::Definition::Transform2DPatch transform2dPatch{};

            if (const auto it = rawActor.FindMember("x"); it != rawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.positionX_ = it->value.GetFloat();
            }

            if (const auto it = rawActor.FindMember("y"); it != rawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.positionY_ = it->value.GetFloat();
            }

            if (const auto it = rawActor.FindMember("transform_rotation_radians");
                it != rawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.rotationRadians_ = it->value.GetFloat();
            }

            if (const auto it = rawActor.FindMember("transform_scale_x");
                it != rawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.scaleX_ = it->value.GetFloat();
            }

            if (const auto it = rawActor.FindMember("transform_scale_y");
                it != rawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.scaleY_ = it->value.GetFloat();
            }

            ContentSpace::Definition::ActorDataPatch actorPatch{};

            actorPatch.transform2dPatch_ = transform2dPatch;

            if (const auto it = rawActor.FindMember("z_order"); it != rawActor.MemberEnd() && it->value.IsInt())
            {
                actorPatch.zOrder_ = it->value.GetInt();
            }

            if (const auto it = rawActor.FindMember("name"); it != rawActor.MemberEnd() && it->value.IsString())
            {
                actorPatch.actorName_ = it->value.GetString();
            }

            if (const auto it = rawActor.FindMember("view_sprite"); it != rawActor.MemberEnd() && it->value.IsString())
            {
                actorPatch.viewSpriteName_ = it->value.GetString();
            }

            return actorPatch;
        }

        void applyTransform2DPatch(
            ObjectSpace::Transform2D&                         outTransform2d,
            const ContentSpace::Definition::Transform2DPatch& patch
        )
        {
            if (patch.positionX_.has_value())
            {
                outTransform2d.position_.x = *(patch.positionX_);
            }

            if (patch.positionY_.has_value())
            {
                outTransform2d.position_.y = *(patch.positionY_);
            }

            if (patch.rotationRadians_.has_value())
            {
                outTransform2d.rotationRadians_ = *patch.rotationRadians_;
            }

            if (patch.scaleX_.has_value())
            {
                outTransform2d.scale_.x = *(patch.scaleX_);
            }

            if (patch.scaleY_.has_value())
            {
                outTransform2d.scale_.y = *(patch.scaleY_);
            }
        }

        void applyActorDataPatch(
            ObjectSpace::DefinitionalActor&                 outDefinition,
            const ContentSpace::Definition::ActorDataPatch& outPatch
        )
        {
            applyTransform2DPatch(outDefinition.transform2d_, outPatch.transform2dPatch_);

            if (outPatch.zOrder_.has_value())
            {
                outDefinition.zOrder_ = *outPatch.zOrder_;
            }

            if (outPatch.actorName_.has_value())
            {
                outDefinition.actorName_ = *outPatch.actorName_;
            }

            if (outPatch.viewSpriteName_.has_value())
            {
                outDefinition.viewSpriteName_ = *outPatch.viewSpriteName_;
            }
        }

        ObjectSpace::DefinitionalActor resolveDefinitionalActor(const ContentSpace::Definition::ActorDataPatch& patch)
        {
            ObjectSpace::DefinitionalActor definition{};
            applyActorDataPatch(definition, patch);
            return definition;
        }

        ObjectSpace::DefinitionalActor resolveDefinitionalActor(
            const ContentSpace::Definition::ActorDataPatch& templatePatch,
            const ContentSpace::Definition::ActorDataPatch& patch
        )
        {
            ObjectSpace::DefinitionalActor definition{};

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
    constexpr std::string_view kFontsDir = "Fonts";
    constexpr std::string_view kScenesDir = "Scenes";
    constexpr std::string_view kTexturesDir = "Textures";

    constexpr std::string_view kGameConfigFileName = "Game";
    constexpr std::string_view kRenderingConfigFileName = "Rendering";
    constexpr std::string_view kSpritesFileName = "Sprites";

    constexpr std::string_view kConfigFilePostfix = ".config";
    constexpr std::string_view kPngPostfix = ".png";
    constexpr std::string_view kScenePostfix = ".scene";
    constexpr std::string_view kSpritePostfix = ".sprite";
    constexpr std::string_view kTemplatePostfix = ".template";
    constexpr std::string_view kTtfPostfix = ".ttf";

    std::string GameContentLoader::createActorTexturePath(std::string_view textureName) const
    {
        Fs::path texturePath;

        if (textureName.empty())
        {
            return texturePath.string();
        }

        texturePath = (gResourceRoot / kTexturesDir / textureName).replace_extension(kPngPostfix);

        return texturePath.string();
    }

    ContentResult GameContentLoader::checkResourceDirectory() const
    {
        if (!Fs::exists(gResourceRoot) || !Fs::is_directory(gResourceRoot))
        {
            return {FileError{"error: Resources/ missing.", FileErrorType::InvalidPath}};
        }

        return {};
    }

    ContentResult GameContentLoader::checkGameConfig() const
    {
        const Fs::path gameConfigPath = (gResourceRoot / kGameConfigFileName).replace_extension(kConfigFilePostfix);

        if (!Fs::exists(gameConfigPath))
        {
            return {FileError{"error: Resources/Game.config missing.", FileErrorType::InvalidPath}};
        }

        return {};
    }

    ContentResult GameContentLoader::loadGameConfig()
    {
        std::ifstream inputFileStream{(gResourceRoot / kGameConfigFileName).replace_extension(kConfigFilePostfix)};

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {ParseError{"error: Resources/Game.config has parse error.", ParseErrorType::ParseFailed}};
        }

        if (!document.IsObject())
        {
            return {ParseError{"error: Resources/Game.config is not object.", ParseErrorType::InvalidStructure}};
        }

        Definition::GameConfigDefinition tempGameConfigDefinition{};

        if (document.HasMember("initial_scene") && document["initial_scene"].IsString())
        {
            tempGameConfigDefinition.initialSceneName_ = std::move(document["initial_scene"].GetString());
        }
        else
        {
            return {ValidationError{"error: initial_scene unspecified.", ValidationErrorType::InvalidData}};
        }

        if (document.HasMember("font") && document["font"].IsString())
        {
            Fs::path fontPath =
                (gResourceRoot / kFontsDir / document["font"].GetString()).replace_extension(kTtfPostfix);

            tempGameConfigDefinition.fontPath_ = std::move(fontPath.string());
        }
        else
        {
            return {ValidationError{"error: font unspecified.", ValidationErrorType::InvalidData}};
        }

        if (document.HasMember("game_title") && document["game_title"].IsString())
        {
            tempGameConfigDefinition.gameTitle_ = std::move(document["game_title"].GetString());
        }

        gameConfigDefinition_ = std::move(tempGameConfigDefinition);

        return {};
    }

    ContentResult GameContentLoader::checkRenderingConfig() const
    {
        const Fs::path renderingConfigPath =
            (gResourceRoot / kRenderingConfigFileName).replace_extension(kConfigFilePostfix);

        if (!Fs::exists(renderingConfigPath))
        {
            return {FileError{"error: Resources/Rendering.config is missing.", FileErrorType::InvalidPath}};
        }

        return {};
    }

    ContentResult GameContentLoader::loadRenderingConfig()
    {
        std::ifstream inputFileStream{(gResourceRoot / kRenderingConfigFileName).replace_extension(kConfigFilePostfix)};

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {ParseError{"error: Resources/Rendering.config has parse error.", ParseErrorType::ParseFailed}};
        }

        if (!document.IsObject())
        {
            return {ParseError{"error: Resources/Rendering.config is not Object.", ParseErrorType::InvalidStructure}};
        }

        Definition::RenderingConfigDefinition tempRenderingConfigDefinition{};

        if (document.HasMember("camera_ortho_width") && document["camera_ortho_width"].IsNumber())
        {
            tempRenderingConfigDefinition.cameraOrthoWidth_ = document["camera_ortho_width"].GetFloat();
        }
        else
        {
            return {ValidationError{
                "error: Resources/Rendering.config has invalid camera_ortho_width.",
                ValidationErrorType::InvalidData
            }};
        }

        if (document.HasMember("x_resolution") && document["x_resolution"].IsInt())
        {
            tempRenderingConfigDefinition.xResolution_ = document["x_resolution"].GetInt();
        }
        else
        {
            std::cout << "info: Resources/Rendering.config has not x_resolution.";
        }

        if (document.HasMember("y_resolution") && document["y_resolution"].IsInt())
        {
            tempRenderingConfigDefinition.yResolution_ = document["y_resolution"].GetInt();
        }
        else
        {
            std::cout << "info: Resources/Rendering.config has not y_resolution.";
        }

        if (document.HasMember("clear_color_r") && document["clear_color_r"].IsInt())
        {
            tempRenderingConfigDefinition.clearColorR_ = document["clear_color_r"].GetInt();
        }

        if (document.HasMember("clear_color_g") && document["clear_color_g"].IsInt())
        {
            tempRenderingConfigDefinition.clearColorG_ = document["clear_color_g"].GetInt();
        }

        if (document.HasMember("clear_color_b") && document["clear_color_b"].IsInt())
        {
            tempRenderingConfigDefinition.clearColorB_ = document["clear_color_b"].GetInt();
        }

        renderingConfigDefinition_ = std::move(tempRenderingConfigDefinition);

        return {};
    }

    ContentResult GameContentLoader::checkSprites() const
    {
        const Fs::path spritePath = (gResourceRoot / kSpritesFileName).replace_extension(kSpritePostfix);

        if (!Fs::exists(spritePath) || !Fs::is_regular_file(spritePath))
        {
            return {FileError{"error: Resources/Sprites.sprite is missing.", FileErrorType::InvalidPath}};
        }

        return {};
    }

    ContentResult GameContentLoader::loadSprites()
    {
        std::ifstream inputFileStream{(gResourceRoot / kSpritesFileName).replace_extension(kSpritePostfix)};

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {ParseError{"error: Resources/Sprites.sprite has parse error.", ParseErrorType::ParseFailed}};
        }

        if (!document.IsObject())
        {
            return {ParseError{"error: Resources/Sprites.sprite is not Object.", ParseErrorType::InvalidStructure}};
        }

        if (!document.HasMember("sprites") || !document["sprites"].IsObject())
        {
            return {ValidationError{
                "error: Resources/Sprites.sprite has invalid sprites.",
                ValidationErrorType::InvalidData
            }};
        }

        const auto& sprites = document["sprites"];

        std::unordered_map<std::string, ViewSpace::Sprite> tempSpritesByName{};
        tempSpritesByName.reserve(sprites.MemberCount());

        for (auto spriteIt = sprites.MemberBegin(); spriteIt != sprites.MemberEnd(); ++spriteIt)
        {
            const auto& sprite = spriteIt->value;

            if (!sprite.IsObject())
            {
                return {ValidationError{"error: sprite is not Object.", ValidationErrorType::InvalidData}};
            }

            const auto textureIt = sprite.FindMember("texture");

            if (textureIt == sprite.MemberEnd() || !textureIt->value.IsString())
            {
                return {ValidationError{"error: sprite's texture is not String.", ValidationErrorType::InvalidData}};
            }

            const auto texturePixelsPerWUIt = sprite.FindMember("texture_pixels_per_world_unit");

            if (texturePixelsPerWUIt == sprite.MemberEnd() || !texturePixelsPerWUIt->value.IsNumber())
            {
                return {ValidationError{
                    "error: sprite's texture_pixels_per_world_unit is not number.",
                    ValidationErrorType::InvalidData
                }};
            }

            const auto normalizedPivotXIt = sprite.FindMember("normalized_pivot_x");
            const auto normalizedPivotYIt = sprite.FindMember("normalized_pivot_y");
            float      normalizedPivotX = 0.5f;
            float      normalizedPivotY = 0.5f;

            if (normalizedPivotXIt != sprite.MemberEnd())
            {
                if (!normalizedPivotXIt->value.IsNumber())
                {
                    return {ValidationError{
                        "error: sprite's normalized_pivot_x is not number.",
                        ValidationErrorType::InvalidData
                    }};
                }

                normalizedPivotX = normalizedPivotXIt->value.GetFloat();
            }

            if (normalizedPivotYIt != sprite.MemberEnd())
            {
                if (!normalizedPivotYIt->value.IsNumber())
                {
                    return {ValidationError{
                        "error: sprite's normalized_pivot_y is not number.",
                        ValidationErrorType::InvalidData
                    }};
                }

                normalizedPivotY = normalizedPivotYIt->value.GetFloat();
            }

            auto spriteValue = ViewSpace::Sprite::createSprite(
                textureIt->value.GetString(),
                texturePixelsPerWUIt->value.GetFloat(),
                glm::fvec2{normalizedPivotX, normalizedPivotY}
            );

            if (!spriteValue)
            {
                return {ValidationError{"error: sprite has invalid data.", ValidationErrorType::InvalidData}};
            }

            tempSpritesByName.emplace(spriteIt->name.GetString(), std::move(*spriteValue));
        }

        sprites_ = std::move(tempSpritesByName);

        return {};
    }

    ContentResult GameContentLoader::checkActorTemplate(std::string_view actorName) const
    {
        const Fs::path templatePath =
            (gResourceRoot / kActorTemplatesDir / actorName).replace_extension(kTemplatePostfix);

        if (!Fs::exists(templatePath) || !Fs::is_regular_file(templatePath))
        {
            return {FileError{std::format("error: template {} is missing.", actorName), FileErrorType::NotFound}};
        }

        return {};
    }

    ContentResult GameContentLoader::loadActorTemplate(std::string_view actorName)
    {
        std::ifstream inputFileStream{
            (gResourceRoot / kActorTemplatesDir / actorName).replace_extension(kTemplatePostfix)
        };

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {ParseError{
                std::format("error: Resources/ActorTemplates/{}{} has parse error.", actorName, kTemplatePostfix),
                ParseErrorType::ParseFailed
            }};
        }

        if (!document.IsObject())
        {
            return {ParseError{
                std::format("error: Resources/ActorTemplates/{}{} is not Object.", actorName, kTemplatePostfix),
                ParseErrorType::InvalidStructure
            }};
        }

        Definition::ActorDataPatch tempActorTemplatePatch = makeActorDataPatch(document);

        actorTemplatePatchesByName_.insert_or_assign(std::string{actorName}, std::move(tempActorTemplatePatch));

        return {};
    }

    ContentResult GameContentLoader::checkSceneResource(std::string_view sceneName) const
    {
        Fs::path scenePath = (gResourceRoot / kScenesDir / std::string{sceneName}).replace_extension(kScenePostfix);

        if (!Fs::exists(scenePath) || !Fs::is_regular_file(scenePath))
        {
            return {FileError{std::format("error: scene {} is missing.", sceneName), FileErrorType::NotFound}};
        }

        return {};
    }

    ContentResult GameContentLoader::loadSceneResource(std::string_view sceneName)
    {
        std::ifstream inputFileStream{
            (gResourceRoot / kScenesDir / std::string{sceneName}).replace_extension(kScenePostfix)
        };

        rapidjson::IStreamWrapper streamWrapper{inputFileStream};

        rapidjson::Document document;

        document.ParseStream(streamWrapper);

        if (document.HasParseError())
        {
            return {ParseError{
                std::format("error: Resources/Scenes/{}.scene has parse error.", sceneName),
                ParseErrorType::ParseFailed
            }};
        }

        if (!document.IsObject())
        {
            return {ParseError{
                std::format("error: Resources/Scenes/{}.scene is not Object.", sceneName),
                ParseErrorType::InvalidStructure
            }};
        }

        if (!document.HasMember("actors") || !document["actors"].IsArray())
        {
            return {ValidationError{
                std::format("error: Resources/Scenes/{}.scene has invalid actors.", sceneName),
                ValidationErrorType::InvalidData
            }};
        }

        const auto& actors = document["actors"];

        std::vector<Definition::ActorDataPatch>     tempRawActors;
        std::vector<ObjectSpace::DefinitionalActor> tempDefinitionalActors;

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
                tempDefinitionalActors.emplace_back(resolveDefinitionalActor(tempRawActors.back()));

                continue;
            }

            if (!templateIt->value.IsString())
            {
                return {
                    ValidationError{"error: actor template property is not String.", ValidationErrorType::InvalidData}
                };
            }

            const std::string_view templateName = templateIt->value.GetString();

            const auto checkTemplateResult = checkActorTemplate(templateName);

            if (!checkTemplateResult.isSucceeded())
            {
                return checkTemplateResult;
            }

            const auto loadTemplateResult = loadActorTemplate(templateName);

            if (!loadTemplateResult.isSucceeded())
            {
                return loadTemplateResult;
            }

            const auto templatePatchIt = actorTemplatePatchesByName_.find(std::string{templateName});

            if (templatePatchIt == actorTemplatePatchesByName_.end())
            {
                return {ValidationError{
                    std::format("error: template {} has no loaded data.", templateName),
                    ValidationErrorType::InvalidData
                }};
            }

            tempDefinitionalActors.emplace_back(
                resolveDefinitionalActor(templatePatchIt->second, tempRawActors.back())
            );
        }

        rawActors_.swap(tempRawActors);
        definitionalActors_.swap(tempDefinitionalActors);

        return {};
    }
} // namespace Uncarved::ContentSpace
