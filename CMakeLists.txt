cmake_minimum_required(VERSION 3.20)
project(NexusHappyReset LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NEXUSHAPPYRESET_CROSS_COMPILE "Cross-compile for Windows using MinGW" ON)

if(NEXUSHAPPYRESET_CROSS_COMPILE)
    if(NOT CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mingw-w64-toolchain.cmake"
            CACHE FILEPATH "MinGW cross-compile toolchain")
    endif()
endif()

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.80
)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    )
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
endif()

file(GLOB TEXTURE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures/*.png")
set(EMBEDDED_ASSETS_DIR "${CMAKE_BINARY_DIR}/generated")
set(EMBEDDED_TEXTURES_CPP "${EMBEDDED_ASSETS_DIR}/EmbeddedTextures.cpp")
set(EMBEDDED_TEXTURES_H "${EMBEDDED_ASSETS_DIR}/EmbeddedTextures.h")

add_custom_command(
    OUTPUT ${EMBEDDED_TEXTURES_CPP} ${EMBEDDED_TEXTURES_H}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${EMBEDDED_ASSETS_DIR}"
    COMMAND python3 "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_assets.py"
            --textures-dir "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
            --output-dir "${EMBEDDED_ASSETS_DIR}"
    DEPENDS ${TEXTURE_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed_assets.py"
    COMMENT "Embedding Happy Reset PNG assets"
    VERBATIM
)

set(SRC_FILES
    src/entry.cpp
    src/core/AppState.cpp
    src/core/SettingsStore.cpp
    src/services/ResetsWatcher.cpp
    src/services/ResetPersistence.cpp
    src/ui/CompassLayout.cpp
    src/ui/DrawUtils.cpp
    src/ui/TextureCatalog.cpp
    src/ui/BouncyChestPanel.cpp
    src/ui/OptionsPanel.cpp
    src/utils/GameBindHelper.cpp
    src/utils/MumbleIdentity.cpp
    ${EMBEDDED_TEXTURES_CPP}
)

add_custom_target(generate_embedded_assets DEPENDS ${EMBEDDED_TEXTURES_CPP} ${EMBEDDED_TEXTURES_H})

add_library(NexusHappyReset SHARED ${SRC_FILES})
add_dependencies(NexusHappyReset generate_embedded_assets)

target_include_directories(NexusHappyReset PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/nexus
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/mumble
    ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(NexusHappyReset PRIVATE imgui nlohmann_json::nlohmann_json d3d11)

target_compile_definitions(NexusHappyReset PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

if(CMAKE_CXX_COMPILER MATCHES "mingw" OR MINGW)
    target_link_options(NexusHappyReset PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

set_target_properties(NexusHappyReset PROPERTIES
    OUTPUT_NAME "NexusHappyReset"
    PREFIX ""
    SUFFIX ".dll"
)
