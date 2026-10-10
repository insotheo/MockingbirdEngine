project(Sandbox LANGUAGES CXX)

find_program(SHADER_COMPILER glslc)

set(SANDBOX_SOURCES
    src/main.cpp
)

add_executable(Sandbox ${SANDBOX_SOURCES})

target_link_libraries(Sandbox
    PRIVATE
        MockingbirdEngineCore
        MockingbirdEngineSDLWindowing
        MockingbirdEngineSDLGraphics
        MockingbirdEngineSDLDearImGui
)

#ASSETS
set(SHADER_SOURCES
    assets/hello.vert
    assets/hello.frag
)
set(SPV_OUTPUTS "")

foreach(SHADER IN LISTS SHADER_SOURCES)
    get_filename_component(SHADER_ABS ${SHADER} ABSOLUTE)
    get_filename_component(SHADER_NAME ${SHADER} NAME)

    set(SPV_OUT "${CMAKE_BINARY_DIR}/bin/assets/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPV_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/assets"
        COMMAND ${SHADER_COMPILER} ${SHADER_ABS} -o ${SPV_OUT}
        DEPENDS ${SHADER_ABS}
        COMMENT "Compiling shader: ${SHADER_NAME} -> ${SHADER_NAME}.spv"
    )

    list(APPEND SPV_OUTPUTS ${SPV_OUT})
endforeach()
target_sources(Sandbox PRIVATE ${SPV_OUTPUTS})

file(GLOB SANDBOX_ASSETS
    assets/testTexture.png
)

if(SANDBOX_ASSETS)
    list(FILTER SANDBOX_ASSETS EXCLUDE REGEX "\\.(frag|vert|glsl)$")
endif()

if(SANDBOX_ASSETS AND NOT "${SANDBOX_ASSETS}" STREQUAL "")
    add_custom_command(
        TARGET Sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/bin/assets"
        COMMAND ${CMAKE_COMMAND} -E copy ${SANDBOX_ASSETS} "${CMAKE_BINARY_DIR}/bin/assets"
        COMMENT "Copying static assets..."
    )
endif()
