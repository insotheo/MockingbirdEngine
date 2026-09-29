project(PumpkinEngineCore VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)

set(ENGINE_CORE
    src/Application.cpp
    src/EventTypeCounter.cpp
    src/SubsystemManager.cpp
    src/FileSys.cpp
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed
)
FetchContent_MakeAvailable(glm)


add_library(PumpkinEngineCore STATIC ${ENGINE_CORE})

target_include_directories(PumpkinEngineCore
    PUBLIC include/
)

target_link_libraries(PumpkinEngineCore
    PUBLIC glm::glm
)

target_compile_definitions(PumpkinEngineCore
    PUBLIC $<$<CONFIG:Debug>:PE_DEBUG> $<$<CONFIG:Release>:PE_RELEASE>
)
