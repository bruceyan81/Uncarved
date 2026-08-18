cmake_minimum_required(VERSION 3.24)

project(Uncarved
    VERSION 0.1.0
    LANGUAGES CXX
)

# Third Party

set(GLM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm")
if(NOT EXISTS "${GLM_ROOT}/glm/glm.hpp")
    message(FATAL_ERROR
        "GLM 1.0.3 was not found. Expected: "
        "${GLM_ROOT}/glm/glm.hpp"
    )
endif()
add_library(glm_headers INTERFACE)
add_library(glm::glm ALIAS glm_headers)
target_include_directories(glm_headers
    SYSTEM INTERFACE
        "${GLM_ROOT}"
)

set(RAPIDJSON_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/rapidjson")
if(NOT EXISTS "${RAPIDJSON_ROOT}/include/rapidjson/document.h")
    message(FATAL_ERROR
        "RapidJSON 1.1.0 was not found. Expected: "
        "${RAPIDJSON_ROOT}/include/rapidjson/document.h"
    )
endif()
add_library(rapidjson_headers INTERFACE)
add_library(rapidjson::rapidjson ALIAS rapidjson_headers)
target_include_directories(rapidjson_headers
    SYSTEM INTERFACE
        "${RAPIDJSON_ROOT}/include"
)

# Boot

add_executable(Uncarved
    src/Main.cpp
    src/Application/Application.cpp
    src/Content/GameContentLoader.cpp
    src/Game/AiController.cpp
    src/Game/GameSimulation.cpp
    src/Game/Game.cpp
    src/Game/GameState.cpp
    src/Game/Interaction.cpp
    src/Input/Input.cpp
    src/Object/Actor.cpp
    src/View/Renderer.cpp
    src/View/Camera.cpp
)

target_compile_features(Uncarved PRIVATE cxx_std_20)

set_target_properties(Uncarved PROPERTIES
    CXX_EXTENSIONS OFF
)

target_link_libraries(Uncarved PRIVATE
    glm::glm
    rapidjson::rapidjson
)

target_include_directories(Uncarved PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(MSVC)
    target_compile_options(Uncarved PRIVATE
        /W4
        /permissive-
        /Zc:__cplusplus
    )
else()
    target_compile_options(Uncarved PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

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
