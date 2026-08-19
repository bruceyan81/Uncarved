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
)

target_include_directories(Uncarved PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(Uncarved SYSTEM PRIVATE
    "${RAPIDJSON_INCLUDE_DIR}"
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
