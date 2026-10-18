#include "GameContentLoader.h"

#include "ContentResult.h"

#include "Logging/Log.h"

#include "Object/Transform2D.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <cmath>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <string_view>
#include <utility>

namespace Uncarved
{
    namespace
    {
        ContentSpace::ContentResult loadActorCollisionShapes(
            const rapidjson::Value&                                  outRawActor,
            std::vector<CollisionSpace::DefinitionalCollisionShape>& outCollisionShapes
        )
        {
            if (const auto it = outRawActor.FindMember("collision_shapes"); it != outRawActor.MemberEnd())
            {
                if (!it->value.IsArray())
                {
                    return {
                        ContentSpace::ValidationError{
                            "ValidationError: Collision shapes is invalid data.",
                            ContentSpace::ValidationErrorType::InvalidData
                        }
                    };
                }

                auto transformCollisionMode = [](std::string_view modeName) {
                    std::optional<CollisionSpace::CollisionMode> result = std::nullopt;
                    if (modeName == "solid")
                    {
                        result = CollisionSpace::CollisionMode::Solid;
                    }
                    else if (modeName == "sensor")
                    {
                        result = CollisionSpace::CollisionMode::Sensor;
                    }
                    return result;
                };

                auto transformCollisionCategory = [](std::string_view categoryName) {
                    std::optional<CollisionSpace::CollisionCategory> result = std::nullopt;
                    if (categoryName == "character")
                    {
                        result = CollisionSpace::CollisionCategory::Character;
                    }
                    else if (categoryName == "world_solid")
                    {
                        result = CollisionSpace::CollisionCategory::WorldSolid;
                    }
                    else if (categoryName == "interactable")
                    {
                        result = CollisionSpace::CollisionCategory::Interactable;
                    }
                    return result;
                };

                const auto& rawCollisionShapes = it->value;
                outCollisionShapes.reserve(outCollisionShapes.size() + rawCollisionShapes.Size());
                std::size_t shapeIndex = 0;

                for (const auto& rawCollisionShape : rawCollisionShapes.GetArray())
                {
                    if (!rawCollisionShape.IsObject())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] is not Object.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto localXIt = rawCollisionShape.FindMember("local_x");
                    const auto localYIt = rawCollisionShape.FindMember("local_y");
                    const auto widthIt = rawCollisionShape.FindMember("width");
                    const auto heightIt = rawCollisionShape.FindMember("height");

                    if (
                        localXIt == rawCollisionShape.MemberEnd() || localYIt == rawCollisionShape.MemberEnd()
                        || widthIt == rawCollisionShape.MemberEnd() || heightIt == rawCollisionShape.MemberEnd()
                        || !localXIt->value.IsNumber() || !localYIt->value.IsNumber()
                        || !widthIt->value.IsNumber() || !heightIt->value.IsNumber()
                        )
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format(
                                    "ValidationError: collision_shapes[{}] has invalid local position or size.",
                                    shapeIndex
                                ),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const float localX = localXIt->value.GetFloat();
                    const float localY = localYIt->value.GetFloat();
                    const float width = widthIt->value.GetFloat();
                    const float height = heightIt->value.GetFloat();

                    if (
                        !std::isfinite(localX) || !std::isfinite(localY)
                        || !std::isfinite(width) || !std::isfinite(height)
                        || width <= 0.0f || height <= 0.0f
                        )
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format(
                                    "ValidationError: collision_shapes[{}] has illegal local position or size.",
                                    shapeIndex
                                ),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto modeIt = rawCollisionShape.FindMember("mode");

                    if (modeIt == rawCollisionShape.MemberEnd() || !modeIt->value.IsString())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] has invalid collision mode.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto collisionMode = transformCollisionMode(modeIt->value.GetString());

                    if (!collisionMode.has_value())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] has unknown collision mode.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto categoryIt = rawCollisionShape.FindMember("category");

                    if (categoryIt == rawCollisionShape.MemberEnd() || !categoryIt->value.IsString())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] has invalid collision category.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto collisionCategory = transformCollisionCategory(categoryIt->value.GetString());

                    if (!collisionCategory.has_value())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] has unknown collision category.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    const auto maskIt = rawCollisionShape.FindMember("mask");

                    if (maskIt == rawCollisionShape.MemberEnd() || !maskIt->value.IsArray())
                    {
                        return {
                            ContentSpace::ValidationError{
                                std::format("ValidationError: collision_shapes[{}] has invalid mask.", shapeIndex),
                                ContentSpace::ValidationErrorType::InvalidData
                            }
                        };
                    }

                    CollisionSpace::CollisionMask mask = 0;

                    for (const auto& rawMaskCategory : maskIt->value.GetArray())
                    {
                        if (!rawMaskCategory.IsString())
                        {
                            return {
                                ContentSpace::ValidationError{
                                    std::format(
                                        "ValidationError: collision_shapes[{}] has invalid mask category.",
                                        shapeIndex
                                    ),
                                    ContentSpace::ValidationErrorType::InvalidData
                                }
                            };
                        }

                        const auto maskCategory = transformCollisionCategory(rawMaskCategory.GetString());

                        if (!maskCategory.has_value())
                        {
                            return {
                                ContentSpace::ValidationError{
                                    std::format(
                                        "ValidationError: collision_shapes[{}] has unknown mask category.",
                                        shapeIndex
                                    ),
                                    ContentSpace::ValidationErrorType::InvalidData
                                }
                            };
                        }

                        mask |= CollisionSpace::makeCollisionMask(*maskCategory);
                    }

                    outCollisionShapes.emplace_back(
                        CollisionSpace::DefinitionalCollisionShape{
                            glm::fvec2{localX, localY},
                            glm::fvec2{width, height},
                            *collisionMode,
                            CollisionSpace::CollisionFilter{*collisionCategory, mask}
                        }
                    );

                    ++shapeIndex;
                }

                return {};
            }
            else
            {
                return {};
            }
        }

        ContentSpace::Definition::ActorDataPatch makeActorDataPatch(const rapidjson::Value& outRawActor)
        {
            ContentSpace::Definition::Transform2DPatch transform2dPatch{};

            if (const auto it = outRawActor.FindMember("x"); it != outRawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.positionX_ = it->value.GetFloat();
            }

            if (const auto it = outRawActor.FindMember("y"); it != outRawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.positionY_ = it->value.GetFloat();
            }

            if (const auto it = outRawActor.FindMember("transform_rotation_radians");
                it != outRawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.rotationRadians_ = it->value.GetFloat();
            }

            if (const auto it = outRawActor.FindMember("transform_scale_x");
                it != outRawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.scaleX_ = it->value.GetFloat();
            }

            if (const auto it = outRawActor.FindMember("transform_scale_y");
                it != outRawActor.MemberEnd() && it->value.IsNumber())
            {
                transform2dPatch.scaleY_ = it->value.GetFloat();
            }

            ContentSpace::Definition::ActorDataPatch actorPatch{};

            actorPatch.transform2dPatch_ = transform2dPatch;

            if (const auto it = outRawActor.FindMember("z_order"); it != outRawActor.MemberEnd() && it->value.IsInt())
            {
                actorPatch.zOrder_ = it->value.GetInt();
            }

            if (const auto it = outRawActor.FindMember("name"); it != outRawActor.MemberEnd() && it->value.IsString())
            {
                actorPatch.actorName_ = it->value.GetString();
            }

            if (const auto it = outRawActor.FindMember("view_sprite"); it != outRawActor.MemberEnd() && it->value.IsString())
            {
                actorPatch.viewSpriteName_ = it->value.GetString();
            }

            return actorPatch;
        }

        void applyTransform2DPatch(
            ObjectSpace::Transform2D&                         outTransform2d,
            const ContentSpace::Definition::Transform2DPatch& outPatch
        )
        {
            if (outPatch.positionX_.has_value())
            {
                outTransform2d.position_.x = *(outPatch.positionX_);
            }

            if (outPatch.positionY_.has_value())
            {
                outTransform2d.position_.y = *(outPatch.positionY_);
            }

            if (outPatch.rotationRadians_.has_value())
            {
                outTransform2d.rotationRadians_ = *outPatch.rotationRadians_;
            }

            if (outPatch.scaleX_.has_value())
            {
                outTransform2d.scale_.x = *(outPatch.scaleX_);
            }

            if (outPatch.scaleY_.has_value())
            {
                outTransform2d.scale_.y = *(outPatch.scaleY_);
            }
        }

        void applyActorDataPatch(
            ObjectSpace::DefinitionalActor&                 outDefinition,
            const ContentSpace::Definition::ActorDataPatch& outPatch
        )
        {
            applyTransform2DPatch(outDefinition.transform2d_, outPatch.transform2dPatch_);

            // TODO テンプレートとシーンの Collision Shape 結合仕様を上書きとクリアの扱いを含めて再検討する予定
            outDefinition.collisionShapes_.insert(
                outDefinition.collisionShapes_.end(),
                outPatch.collisionShapes_.begin(),
                outPatch.collisionShapes_.end()
            );

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
            const ContentSpace::Definition::ActorDataPatch& outTemplatePatch,
            const ContentSpace::Definition::ActorDataPatch& outPatch
        )
        {
            ObjectSpace::DefinitionalActor definition{};

            applyActorDataPatch(definition, outTemplatePatch);
            applyActorDataPatch(definition, outPatch);

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
            return {
                FileError{
                    "FileError: Resources/ missing.",
                    FileErrorType::InvalidPath
                }
            };
        }

        return {};
    }

    ContentResult GameContentLoader::checkGameConfig() const
    {
        const Fs::path gameConfigPath = (gResourceRoot / kGameConfigFileName).replace_extension(kConfigFilePostfix);

        if (!Fs::exists(gameConfigPath))
        {
            return {
                FileError{
                    "FileError: Resources/Game.config missing.",
                    FileErrorType::InvalidPath
                }
            };
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
            return {
                ParseError{
                    "ParseError: Resources/Game.config has parse error.",
                    ParseErrorType::ParseFailed
                }
            };
        }

        if (!document.IsObject())
        {
            return {
                ParseError{
                    "ParseError: Resources/Game.config is not object.",
                    ParseErrorType::InvalidStructure
                }
            };
        }

        Definition::GameConfigDefinition tempGameConfigDefinition{};

        if (document.HasMember("initial_scene") && document["initial_scene"].IsString())
        {
            tempGameConfigDefinition.initialSceneName_ = std::move(document["initial_scene"].GetString());
        }
        else
        {
            return {
                ValidationError{
                    "ValidationError: Initial_scene unspecified.",
                    ValidationErrorType::InvalidData
                }
            };
        }

        if (document.HasMember("font") && document["font"].IsString())
        {
            Fs::path fontPath =
                (gResourceRoot / kFontsDir / document["font"].GetString()).replace_extension(kTtfPostfix);

            tempGameConfigDefinition.fontPath_ = std::move(fontPath.string());
        }
        else
        {
            return {
                ValidationError{
                    "ValidationError: Font unspecified.",
                    ValidationErrorType::InvalidData
                }
            };
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
            return {
                FileError{
                    "FileError: Resources/Rendering.config is missing.",
                    FileErrorType::InvalidPath
                }
            };
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
            return {
                ParseError{
                    "ParseError: Resources/Rendering.config has parse error.",
                    ParseErrorType::ParseFailed
                }
            };
        }

        if (!document.IsObject())
        {
            return {
                ParseError{
                    "ParseError: Resources/Rendering.config is not Object.",
                    ParseErrorType::InvalidStructure
                }
            };
        }

        Definition::RenderingConfigDefinition tempRenderingConfigDefinition{};

        if (document.HasMember("camera_ortho_width") && document["camera_ortho_width"].IsNumber())
        {
            tempRenderingConfigDefinition.cameraOrthoWidth_ = document["camera_ortho_width"].GetFloat();
        }
        else
        {
            return {
                ValidationError{
                    "ValidationError: Resources/Rendering.config has invalid camera_ortho_width.",
                    ValidationErrorType::InvalidData
                }
            };
        }

        if (document.HasMember("x_resolution") && document["x_resolution"].IsInt())
        {
            tempRenderingConfigDefinition.xResolution_ = document["x_resolution"].GetInt();
        }
        else
        {
            UC_LOG(gContent, LogVerbosity::Info, "Resources/Rendering.config has not x_resolution.");
        }

        if (document.HasMember("y_resolution") && document["y_resolution"].IsInt())
        {
            tempRenderingConfigDefinition.yResolution_ = document["y_resolution"].GetInt();
        }
        else
        {
            UC_LOG(gContent, LogVerbosity::Info, "Resources/Rendering.config has not y_resolution.");
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
            return {
                FileError{
                    "FileError: Resources/Sprites.sprite is missing.",
                    FileErrorType::InvalidPath
                }
            };
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
            return {
                ParseError{
                    "ParseError: Resources/Sprites.sprite has parse error.",
                    ParseErrorType::ParseFailed
                }
            };
        }

        if (!document.IsObject())
        {
            return {
                ParseError{
                    "ParseError: Resources/Sprites.sprite is not Object.",
                    ParseErrorType::InvalidStructure
                }
            };
        }

        if (!document.HasMember("sprites") || !document["sprites"].IsObject())
        {
            return {
                ValidationError{
                    "ValidationError: Resources/Sprites.sprite has invalid sprites.",
                    ValidationErrorType::InvalidData
                }
            };
        }

        const auto& sprites = document["sprites"];

        std::unordered_map<std::string, ViewSpace::Sprite> tempSpritesByName{};
        tempSpritesByName.reserve(sprites.MemberCount());

        for (auto spriteIt = sprites.MemberBegin(); spriteIt != sprites.MemberEnd(); ++spriteIt)
        {
            const auto& sprite = spriteIt->value;

            if (!sprite.IsObject())
            {
                return {
                    ValidationError{
                        "ValidationError: Sprite is not Object.",
                        ValidationErrorType::InvalidData
                    }
                };
            }

            const auto textureIt = sprite.FindMember("texture");

            if (textureIt == sprite.MemberEnd() || !textureIt->value.IsString())
            {
                return {
                    ValidationError{
                        "ValidationError: Sprite's texture is not String.",
                        ValidationErrorType::InvalidData
                    }
                };
            }

            const auto texturePixelsPerWUIt = sprite.FindMember("texture_pixels_per_world_unit");

            if (texturePixelsPerWUIt == sprite.MemberEnd() || !texturePixelsPerWUIt->value.IsNumber())
            {
                return {
                    ValidationError{
                        "ValidationError: Sprite's texture_pixels_per_world_unit is not number.",
                        ValidationErrorType::InvalidData
                    }
                };
            }

            const auto normalizedPivotXIt = sprite.FindMember("normalized_pivot_x");
            const auto normalizedPivotYIt = sprite.FindMember("normalized_pivot_y");
            float      normalizedPivotX = 0.5f;
            float      normalizedPivotY = 0.5f;

            if (normalizedPivotXIt != sprite.MemberEnd())
            {
                if (!normalizedPivotXIt->value.IsNumber())
                {
                    return {
                        ValidationError{
                            "ValidationError: Sprite's normalized_pivot_x is not number.",
                            ValidationErrorType::InvalidData
                        }
                    };
                }

                normalizedPivotX = normalizedPivotXIt->value.GetFloat();
            }

            if (normalizedPivotYIt != sprite.MemberEnd())
            {
                if (!normalizedPivotYIt->value.IsNumber())
                {
                    return {
                        ValidationError{
                            "ValidationError: Sprite's normalized_pivot_y is not number.",
                            ValidationErrorType::InvalidData
                        }
                    };
                }

                normalizedPivotY = normalizedPivotYIt->value.GetFloat();
            }

            const auto sourceXIt = sprite.FindMember("source_x");
            const auto sourceYIt = sprite.FindMember("source_y");
            const auto sourceWidthIt = sprite.FindMember("source_width");
            const auto sourceHeightIt = sprite.FindMember("source_height");

            ViewSpace::SpriteRegion spriteRegion{};

            bool bHasRegion = false;

            if (sourceXIt != sprite.MemberEnd() && sourceYIt != sprite.MemberEnd()
                && sourceWidthIt != sprite.MemberEnd() && sourceHeightIt != sprite.MemberEnd())
            {
                if (!sourceXIt->value.IsInt() || !sourceYIt->value.IsInt()
                    || !sourceWidthIt->value.IsInt() || !sourceHeightIt->value.IsInt())
                {
                    return {
                        ValidationError{
                            "ValidationError: Sprite Region's member are not number.",
                            ValidationErrorType::InvalidData
                        }
                    };
                }
                else
                {
                    int x = sourceXIt->value.GetInt();
                    int y = sourceYIt->value.GetInt();
                    int width = sourceWidthIt->value.GetInt();
                    int height = sourceHeightIt->value.GetInt();

                    if (x < 0 || y < 0 || width <= 0 || height <= 0)
                    {
                        return {
                            ValidationError{
                                "ValidationError: Sprite Region is illegal.",
                                ValidationErrorType::InvalidData
                            }
                        };
                    }
                    else
                    {
                        spriteRegion.x_ = x;
                        spriteRegion.y_ = y;
                        spriteRegion.width_ = width;
                        spriteRegion.height_ = height;
                        bHasRegion = true;
                    }
                }
            }

            std::optional<ViewSpace::Sprite> spriteValue;

            if (bHasRegion)
            {
                spriteValue = ViewSpace::Sprite::createSprite(
                    textureIt->value.GetString(),
                    texturePixelsPerWUIt->value.GetFloat(),
                    glm::fvec2{normalizedPivotX, normalizedPivotY},
                    spriteRegion
                );
            }
            else
            {
                spriteValue = ViewSpace::Sprite::createSprite(
                    textureIt->value.GetString(),
                    texturePixelsPerWUIt->value.GetFloat(),
                    glm::fvec2{normalizedPivotX, normalizedPivotY}
                );
            }

            if (!spriteValue)
            {
                return {
                    ValidationError{
                        "ValidationError: Sprite has invalid data.",
                        ValidationErrorType::InvalidData
                    }
                };
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
            return {
                FileError{
                    std::format("FileError: Template {} is missing.", actorName),
                    FileErrorType::NotFound
                }
            };
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
            return {
                ParseError{
                    std::format("ParseError: Resources/ActorTemplates/{}{} has parse error.", actorName, kTemplatePostfix),
                    ParseErrorType::ParseFailed
                }
            };
        }

        if (!document.IsObject())
        {
            return {
                ParseError{
                    std::format("ParseError: Resources/ActorTemplates/{}{} is not Object.", actorName, kTemplatePostfix),
                    ParseErrorType::InvalidStructure
                }
            };
        }

        Definition::ActorDataPatch tempActorTemplatePatch = makeActorDataPatch(document);

        const auto collisionShapesResult = loadActorCollisionShapes(document, tempActorTemplatePatch.collisionShapes_);

        if (!collisionShapesResult.isSucceeded())
        {
            return collisionShapesResult;
        }

        actorTemplatePatchesByName_.insert_or_assign(std::string{actorName}, std::move(tempActorTemplatePatch));

        return {};
    }

    ContentResult GameContentLoader::checkSceneResource(std::string_view sceneName) const
    {
        Fs::path scenePath = (gResourceRoot / kScenesDir / std::string{sceneName}).replace_extension(kScenePostfix);

        if (!Fs::exists(scenePath) || !Fs::is_regular_file(scenePath))
        {
            return {
                FileError{
                    std::format("FileError: Scene {} is missing.", sceneName),
                    FileErrorType::NotFound
                }
            };
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
            return {
                ParseError{
                    std::format("ParseError: Resources/Scenes/{}.scene has parse error.", sceneName),
                    ParseErrorType::ParseFailed
                }
            };
        }

        if (!document.IsObject())
        {
            return {
                ParseError{
                    std::format("ParseError: Resources/Scenes/{}.scene is not Object.", sceneName),
                    ParseErrorType::InvalidStructure
                }
            };
        }

        if (!document.HasMember("actors") || !document["actors"].IsArray())
        {
            return {
                ValidationError{
                    std::format("ValidationError: Resources/Scenes/{}.scene has invalid actors.", sceneName),
                    ValidationErrorType::InvalidData
                }
            };
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

            auto actorPatch = makeActorDataPatch(rawActor);
            const auto collisionShapesResult = loadActorCollisionShapes(rawActor, actorPatch.collisionShapes_);

            if (!collisionShapesResult.isSucceeded())
            {
                return collisionShapesResult;
            }

            tempRawActors.emplace_back(actorPatch);

            const auto templateIt = rawActor.FindMember("template");

            if (templateIt == rawActor.MemberEnd())
            {
                tempDefinitionalActors.emplace_back(resolveDefinitionalActor(tempRawActors.back()));

                continue;
            }

            if (!templateIt->value.IsString())
            {
                return {
                    ValidationError{
                        "ValidationError: Actor template property is not String.",
                        ValidationErrorType::InvalidData
                    }
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
                return {
                    ValidationError{
                        std::format("ValidationError: Template {} has no loaded data.", templateName),
                        ValidationErrorType::InvalidData
                    }
                };
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
