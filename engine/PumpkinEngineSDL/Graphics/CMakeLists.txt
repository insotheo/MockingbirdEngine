project(PumpkinEngineSDLGraphics VERSION 0.0.1 LANGUAGES CXX)

set(SubsystemSources
    src/Subsystem.cpp
    src/Renderer.cpp
    src/RendererMaterials.cpp
)

add_library(PumpkinEngineSDLGraphics STATIC ${SubsystemSources})

target_link_libraries(PumpkinEngineSDLGraphics
    PUBLIC
        PumpkinEngineCore
        SDL3::SDL3
)

target_include_directories(PumpkinEngineSDLGraphics
    PUBLIC include/
)
