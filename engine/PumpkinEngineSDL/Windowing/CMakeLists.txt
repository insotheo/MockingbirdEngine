project(PumpkinEngineSDLWindowing VERSION 0.0.1 LANGUAGES CXX)

set(SubsystemSources
    src/Subsystem.cpp
)

add_library(PumpkinEngineSDLWindowing STATIC ${SubsystemSources})

target_link_libraries(PumpkinEngineSDLWindowing
    PUBLIC
        PumpkinEngineCore
        SDL3::SDL3
)

target_include_directories(PumpkinEngineSDLWindowing
    PUBLIC include/
)
