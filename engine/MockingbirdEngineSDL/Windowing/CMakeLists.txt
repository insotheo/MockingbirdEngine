project(MockingbirdEngineSDLWindowing VERSION 0.0.1 LANGUAGES CXX)

set(SubsystemSources
    src/Subsystem.cpp
)

add_library(MockingbirdEngineSDLWindowing STATIC ${SubsystemSources})

target_link_libraries(MockingbirdEngineSDLWindowing
    PUBLIC
        MockingbirdEngineCore
        SDL3::SDL3
)

target_include_directories(MockingbirdEngineSDLWindowing
    PUBLIC include/
)
