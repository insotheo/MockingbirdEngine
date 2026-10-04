project(MockingbirdEngineCore VERSION 0.0.1 LANGUAGES CXX)

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


add_library(MockingbirdEngineCore STATIC ${ENGINE_CORE})

target_include_directories(MockingbirdEngineCore
    PUBLIC include/
)

target_link_libraries(MockingbirdEngineCore
    PUBLIC glm::glm
)

target_compile_definitions(MockingbirdEngineCore
    PUBLIC $<$<CONFIG:Debug>:ME_DEBUG> $<$<CONFIG:Release>:ME_RELEASE>
)
