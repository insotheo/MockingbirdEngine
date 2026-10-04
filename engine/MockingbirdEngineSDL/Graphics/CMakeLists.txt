project(MockingbirdEngineSDLGraphics VERSION 0.0.1 LANGUAGES CXX)

set(SubsystemSources
    src/Subsystem.cpp
    src/Renderer.cpp
    src/RendererMaterials.cpp
)

add_library(MockingbirdEngineSDLGraphics STATIC ${SubsystemSources})

target_link_libraries(MockingbirdEngineSDLGraphics
    PUBLIC
        MockingbirdEngineCore
        SDL3::SDL3
)

target_include_directories(MockingbirdEngineSDLGraphics
    PUBLIC include/
)
