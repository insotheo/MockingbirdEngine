project(Sandbox LANGUAGES CXX)

set(SANDBOX_SOURCES
    src/main.cpp
)

add_executable(Sandbox ${SANDBOX_SOURCES})

target_link_libraries(Sandbox
    PRIVATE
        PumpkinEngineCore
        PumpkinEngineSDLWindowing
        PumpkinEngineSDLGraphics
        PumpkinEngineSDLDearImGui
)

#ASSETS
file(GLOB SANDBOX_ASSETS
assets/*.spv
)

add_custom_command(
    TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${SANDBOX_ASSETS}
        ${CMAKE_BINARY_DIR}/bin
    COMMENT "Copying assets..."
)
