project(MockingbirdEngineSDLGraphics VERSION 0.0.1 LANGUAGES CXX)

#SPIRV-CROSS
set(SPIRV_CROSS_CLI OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_STATIC ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_SHARED OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    spirv-cross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
    GIT_TAG aa217aeb6c9f0ace7a0ab233b28807edf45eb165
)
FetchContent_MakeAvailable(spirv-cross)


set(SubsystemSources
    src/Subsystem.cpp
    src/Renderer.cpp
    src/RendererMaterials.cpp
    src/RendererShadersReflection.cpp
)

add_library(MockingbirdEngineSDLGraphics STATIC ${SubsystemSources})

target_link_libraries(MockingbirdEngineSDLGraphics
    PUBLIC
        MockingbirdEngineCore
        SDL3::SDL3
        spirv-cross-core
        spirv-cross-glsl
)

target_include_directories(MockingbirdEngineSDLGraphics
    PUBLIC include/
)
