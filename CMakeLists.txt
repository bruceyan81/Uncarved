cmake_minimum_required(VERSION 3.24)

project(Uncarved
    VERSION 0.1.0
    LANGUAGES CXX
)

# Third Party

find_package(spdlog CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

find_path(RAPIDJSON_INCLUDE_DIR
    NAMES rapidjson/document.h
    REQUIRED
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)

# Logging

add_library(UncarvedLogging STATIC
    src/Logging/Log.cpp
    src/Logging/LogCategories.cpp
)

target_link_libraries(UncarvedLogging
    PRIVATE
        spdlog::spdlog
)

target_include_directories(UncarvedLogging PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(UncarvedLogging PUBLIC cxx_std_20)

# Platform

add_library(UncarvedPlatform STATIC
    src/Platform/Audio/AudioDevice.cpp
    src/Platform/Audio/SoundSource.cpp
    src/Platform/Audio/SoundWave.cpp
    src/Platform/Audio/SoundWaveStore.cpp
    src/Platform/EventPump.cpp
    src/Platform/Graphics.cpp
    src/Platform/PlatformRuntime.cpp
    src/Platform/Text.cpp
    src/Platform/TextRuntime.cpp
)

target_link_libraries(UncarvedPlatform
    PUBLIC
        glm::glm
    PRIVATE
        SDL3::SDL3
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
        SDL3_ttf::SDL3_ttf
        $<IF:$<TARGET_EXISTS:SDL3_mixer::SDL3_mixer>,SDL3_mixer::SDL3_mixer,SDL3_mixer::SDL3_mixer-static>
        UncarvedLogging
)

target_include_directories(UncarvedPlatform PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(UncarvedPlatform PUBLIC cxx_std_20)

# Core

add_library(UncarvedCore STATIC
    src/Application/ApplicationCore.cpp
    src/Application/EntryPoint.cpp
    src/Content/GameContentLoader.cpp
    src/Game/CommandBuffer.cpp
    src/Game/Runtime.cpp
    src/Game/World.cpp
    src/Input/InputSystem.cpp
    src/Object/Actor.cpp
    src/Time/AppTime.cpp
    src/Time/WorldTime.cpp
    src/View/Camera2D.cpp
    src/View/SceneRenderer.cpp
)

target_link_libraries(UncarvedCore
    PUBLIC
        glm::glm
        UncarvedPlatform
    PRIVATE
        UncarvedLogging
)

target_include_directories(UncarvedCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(UncarvedCore SYSTEM PRIVATE
    "${RAPIDJSON_INCLUDE_DIR}"
)

target_compile_features(UncarvedCore PUBLIC cxx_std_20)

# Sandbox

add_subdirectory(Sandbox)

set_property(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    PROPERTY VS_STARTUP_PROJECT Sandbox
)

foreach(target IN ITEMS
    UncarvedCore
    UncarvedPlatform
    UncarvedLogging
    Sandbox
)
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            /Zc:__cplusplus
            /Zc:preprocessor
            /utf-8
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endif()
endforeach()
