project(PumpkinEngineSDL VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)

set(SDL_TESTS OFF CACHE BOOL "Disable SDL3 tests" FORCE)
set(SDL_DOCUMENTATION OFF CACHE BOOL "Disable SDL3 documentation" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Disable shared SDL3" FORCE)
set(SDL_STATIC ON CACHE BOOL "Enable static SDL3" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.4.16
)
FetchContent_MakeAvailable(SDL3)

add_subdirectory(Windowing)
add_subdirectory(Graphics)
