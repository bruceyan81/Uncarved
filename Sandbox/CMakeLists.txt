# Sandbox

add_executable(Sandbox
    src/Main.cpp
    src/SandboxApplication.cpp
)

target_link_libraries(Sandbox
    PRIVATE
        UncarvedCore
        UncarvedLogging
)

target_compile_features(Sandbox PRIVATE cxx_std_20)

add_custom_target(SyncSandboxResources ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        "$<TARGET_FILE_DIR:Sandbox>/Resources"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:Sandbox>/Resources"

    COMMENT "Syncing Sandbox resources."
    VERBATIM
)

add_dependencies(Sandbox SyncSandboxResources)
