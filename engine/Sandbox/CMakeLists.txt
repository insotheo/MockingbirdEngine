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
)
