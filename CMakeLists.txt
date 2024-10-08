cmake_minimum_required(VERSION 3.22)
project(blipcade_cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

Set(FETCHCONTENT_QUIET FALSE)

include(FetchContent)

# Add the directory containing your header files
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-renderer)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-api)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-runtime)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-loader)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-ecs)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-devtool)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-collision)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-audio)
include_directories(${CMAKE_SOURCE_DIR}/external/quickjs-patched)
include_directories(${CMAKE_SOURCE_DIR}/external/earcut)
include_directories(${CMAKE_SOURCE_DIR}/external/ImGuiFileDialog)


FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2  # Specify the version you need
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(json)

FetchContent_Declare(
        dynamic_bitset
        GIT_REPOSITORY https://github.com/pinam45/dynamic_bitset.git
        GIT_TAG v1.3.0
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(dynamic_bitset)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

if (DEFINED EMSCRIPTEN)
add_compile_definitions(GRAPHICS_API_OPENGL_ES3) # use OpenGL ES 3.0
set(PLATFORM "Web" CACHE STRING "" FORCE) # build for web
endif ()

FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

# Add Dear ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
        GIT_TAG "v1.91.2"
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        GIT_PROGRESS TRUE
)

FetchContent_Populate(imgui)

# Add rlImGui
FetchContent_Declare(
        rlImGui
        GIT_REPOSITORY "https://github.com/raylib-extras/rlImGui.git"
        GIT_TAG "main"
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui
        GIT_PROGRESS TRUE
)

FetchContent_Populate(rlImGui)

# Add your source files
set(SOURCES
        main.cpp
        src/blipcade-renderer/renderer.cpp
        src/blipcade-renderer/palette.cpp
        src/blipcade-renderer/color.cpp
        src/blipcade-renderer/font.cpp
        src/blipcade-renderer/spritesheet.cpp
        src/blipcade-renderer/canvas.cpp
        src/blipcade-api/converters.cpp
        src/blipcade-runtime/runtime.cpp
        src/blipcade-runtime/JsBindings.cpp
        src/blipcade-runtime/keystate.cpp
        src/blipcade-loader/cartridge.cpp
        src/blipcade-runtime/mousestate.cpp
        src/blipcade-ecs/ECS.cpp
        src/blipcade-devtool/devtool.cpp
        src/blipcade-devtool/polygonEditor.cpp
        src/blipcade-collision/collision.cpp
        src/blipcade-collision/collider.cpp
        src/blipcade-audio/audio.cpp
        src/blipcade-audio/effect.cpp
        src/blipcade-audio/reverb.cpp
        src/blipcade-audio/echo.cpp
        src/blipcade-collision/triangulation.cpp
        src/blipcade-collision/navmesh.cpp
        src/blipcade-collision/pathfinding.cpp
        external/ImGuiFileDialog/ImGuilFileDialog.cpp
        src/blipcade-devtool/spriteEditor.cpp
        src/blipcade-loader/project.cpp
)

# Path to the JSON file
set(JSON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/carts/scene-test-build.json")

# Path to the header template
set(HEADER_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/json_cart.hpp.in")

# Path to the generated header
set(GENERATED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/json_cart_data.hpp")

# Path to the CMake script
set(GENERATE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generate_header.cmake")

# Add a custom command to generate data.hpp from data.json
add_custom_command(
        OUTPUT "${GENERATED_HEADER}"
        COMMAND ${CMAKE_COMMAND} -DJSON_FILE="${JSON_FILE}" -DHEADER_FILE="${GENERATED_HEADER}" -P "${GENERATE_SCRIPT}"
        DEPENDS "${JSON_FILE}" "${HEADER_TEMPLATE}" "${GENERATE_SCRIPT}"
        COMMENT "Generating json_cart_data.hpp from src/cart.json"
)

# Add a custom target that depends on the generated header
add_custom_target(generate_data_header DEPENDS "${GENERATED_HEADER}")
#add_dependencies(generate_data_header build_carts)

# Add QuickJS source files
set(QUICKJS_SOURCES
        external/quickjs-patched/libbf.c
        external/quickjs-patched/quickjs.c
        external/quickjs-patched/libregexp.c
        external/quickjs-patched/libunicode.c
        external/quickjs-patched/cutils.c
        external/quickjs-patched/quickjs-libc.c
        external/quickjs-patched/cutils.c
)

#if ("${CMAKE_SYSTEM}" MATCHES "Linux" OR "${CMAKE_SYSTEM}" MATCHES "Darwin-*")
#    find_package(OpenGL REQUIRED)
#    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#endif ()

# Compile QuickJS as a separate target
add_library(quickjs STATIC ${QUICKJS_SOURCES})
target_compile_definitions(quickjs PRIVATE CONFIG_VERSION="${QUICKJS_VERSION}")

# Add compiler flags for QuickJS
target_compile_options(quickjs PRIVATE
        -D_GNU_SOURCE
        -DCONFIG_VERSION="${QUICKJS_VERSION}"
        -DCONFIG_BIGNUM
        -Wno-implicit-function-declaration
        -Wno-expansion-to-defined
        -Wno-nullability-completeness
)

# If using Clang, add these flags to fix compatibility issues
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(quickjs PRIVATE
            -Wno-unknown-warning-option
            -Wno-implicit-const-int-float-conversion
            -Wno-sign-conversion
    )
endif ()

# Create the main executable target
add_executable(blipcade_cmake ${SOURCES})

# custom target to build carts
add_custom_target(build_carts ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Building carts..."
        COMMAND CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/tools/build-carts.sh
        COMMENT "Building carts"
)

# Add custom commands to generate shader headers
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/shader.vert
        ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        DEPENDS src/blipcade-renderer/shaders/shader.vert
        COMMENT "Generating vertex shader header"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/shader.frag
        ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
        DEPENDS src/blipcade-renderer/shaders/shader.frag
        COMMENT "Generating fragment shader header"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/palette_shader.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/palette_shader.frag
        ${CMAKE_CURRENT_BINARY_DIR}/palette_shader.h
        DEPENDS src/blipcade-renderer/shaders/palette_shader.frag
        COMMENT "Generating palette fragment shader header"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader_vert.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/lighting_shader.vert
        ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader_vert.h
        DEPENDS src/blipcade-renderer/shaders/lighting_shader.vert
        COMMENT "Generating vertex shader header"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/blipcade-renderer/shaders/lighting_shader.frag
        ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader.h
        DEPENDS src/blipcade-renderer/shaders/lighting_shader.frag
        COMMENT "Generating lighting fragment shader header"
)

# Add the generated headers to your target's sources
target_sources(blipcade_cmake PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
        ${CMAKE_CURRENT_BINARY_DIR}/palette_shader.h
        ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader.h
        ${CMAKE_CURRENT_BINARY_DIR}/lighting_shader_vert.h
)

# Make sure the binary dir is in the include path
target_include_directories(blipcade_cmake PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_library(imgui_rl STATIC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
)

add_library(rlImGui STATIC
        external/rlImGui/rlImGui.cpp
)


target_include_directories(imgui_rl
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/external/imgui
        ${CMAKE_CURRENT_LIST_DIR}/external/imgui/backends
)

target_include_directories(rlImGui
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/external/rlImGui
        ${CMAKE_CURRENT_LIST_DIR}/external/imgui
        ${CMAKE_CURRENT_LIST_DIR}/external/imgui/backends

        # Include directories for raylib
        ${raylib_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(blipcade_cmake quickjs raylib imgui_rl rlImGui ${OPENGL_LIBRARIES} nlohmann_json::nlohmann_json sul::dynamic_bitset)

# Detect if building with Emscripten
if (DEFINED EMSCRIPTEN)
    set(USE_WAYLAND_DISPLAY OFF CACHE BOOL "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Oz")
    list(REMOVE_ITEM example_sources ${CMAKE_CURRENT_SOURCE_DIR}/others/rlgl_standalone.c)

    set_target_properties(blipcade_cmake
            PROPERTIES SUFFIX ".html"
            LINK_FLAGS " --bind -s WASM=2 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576 -sFULL_ES3 --preload-file ${CMAKE_SOURCE_DIR}/src/resources@/resources"
            )
    string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS}")

    message(STATUS "Configuring for Emscripten")
else ()
    message(STATUS "Configuring for Desktop")
endif (DEFINED EMSCRIPTEN)

# Find Python
find_package(Python COMPONENTS Interpreter REQUIRED)

# Add a custom command to run the Python JSDoc extractor
add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/js_api_docs.md
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/tools/extract_jsdoc.py
        ${CMAKE_SOURCE_DIR}/src/blipcade-runtime/JsBindings.cpp
        -o ${CMAKE_SOURCE_DIR}/js_api_docs.md
        DEPENDS ${CMAKE_SOURCE_DIR}/src/blipcade-runtime/JsBindings.cpp
        ${CMAKE_SOURCE_DIR}/src/tools/extract_jsdoc.py
        COMMENT "Extracting JavaScript API documentation"
)

add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/blipcade.d.ts
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/tools/extract_d_ts.py
        ${CMAKE_SOURCE_DIR}/src/blipcade-runtime/JsBindings.cpp
        -o ${CMAKE_SOURCE_DIR}/blipcade.d.ts
        DEPENDS ${CMAKE_SOURCE_DIR}/src/blipcade-runtime/JsBindings.cpp
        ${CMAKE_SOURCE_DIR}/src/tools/extract_d_ts.py
        COMMENT "Extracting TypeScript definitions"
)

# Copy "resources" folder to build directory
file(COPY ${CMAKE_SOURCE_DIR}/src/resources DESTINATION ${CMAKE_BINARY_DIR})

# Add a custom target for documentation generation
add_custom_target(generate_js_docs DEPENDS ${CMAKE_BINARY_DIR}/js_api_docs.md)

# Add a custom target for TypeScript definitions
add_custom_target(generate_d_ts DEPENDS ${CMAKE_BINARY_DIR}/blipcade.d.ts)

# Make the main target depend on the documentation
add_dependencies(blipcade_cmake generate_js_docs generate_d_ts generate_data_header)