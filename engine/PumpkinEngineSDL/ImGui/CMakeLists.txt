project(PumpkinEngineSDLDearImGui VERSION 0.0.1 LANGUAGES CXX)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.92.9b
)
FetchContent_MakeAvailable(imgui)

#making imgui
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PRIVATE SDL3::SDL3)

#then make library
set(SubsystemSources
    src/Subsystem.cpp
)

add_library(PumpkinEngineSDLDearImGui STATIC ${SubsystemSources})

target_link_libraries(PumpkinEngineSDLDearImGui
    PUBLIC
        PumpkinEngineCore
        PumpkinEngineSDLGraphics
        SDL3::SDL3
        imgui_lib
)

target_include_directories(PumpkinEngineSDLDearImGui
    PUBLIC include/
)
