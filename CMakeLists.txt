cmake_minimum_required(VERSION 3.24)

project(Uncarved
    VERSION 0.1.0
    LANGUAGES CXX
)

# Third Party

find_package(glm CONFIG REQUIRED)

find_path(RAPIDJSON_INCLUDE_DIR
    NAMES rapidjson/document.h
    REQUIRED
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(SDL3_mixer CONFIG REQUIRED)

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
)

target_include_directories(UncarvedPlatform PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(UncarvedPlatform PUBLIC cxx_std_20)

# Core

add_library(UncarvedCore STATIC
    src/Application/Application.cpp
    src/Content/GameContentLoader.cpp
    src/Game/AiController.cpp
    src/Game/GameSimulation.cpp
    src/Game/CommandBuffer.cpp
    src/Game/Game.cpp
    src/Game/GameState.cpp
    src/Game/World.cpp
    src/Input/Input.cpp
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
)

target_include_directories(UncarvedCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(UncarvedCore SYSTEM PRIVATE
    "${RAPIDJSON_INCLUDE_DIR}"
)

target_compile_features(UncarvedCore PUBLIC cxx_std_20)

# Boot

add_executable(Uncarved
    src/Main.cpp
)

target_link_libraries(Uncarved PRIVATE
    UncarvedCore
)

target_compile_features(Uncarved PRIVATE cxx_std_20)

foreach(target IN ITEMS UncarvedCore UncarvedPlatform Uncarved)
    set_target_properties(${target} PROPERTIES
        CXX_EXTENSIONS OFF
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            /Zc:__cplusplus
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

add_custom_target(SyncResources ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "$<TARGET_FILE_DIR:Uncarved>/Resources"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:Uncarved>/Resources"

    COMMENT "Syncing Resources."
    VERBATIM
)

add_dependencies(Uncarved SyncResources)
