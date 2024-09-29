cmake_minimum_required(VERSION 3.22)
project(blipcade_cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Add the directory containing your header files
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-renderer)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-api)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-runtime)
include_directories(${CMAKE_SOURCE_DIR}/src/blipcade-loader)
include_directories(${CMAKE_SOURCE_DIR}/external/quickjs-patched)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2  # Specify the version you need
)

FetchContent_MakeAvailable(json)


if (NOT DEFINED EMSCRIPTEN) # For Emscripten, we don't need to fetch GLFW, it's already included.
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)
endif()

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
        src/blipcade-loader/cartridge.h
)

# Path to the JSON file
set(JSON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/carts/testcart-build.json")

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
add_dependencies(generate_data_header build_carts)

# Add QuickJS source files
set(QUICKJS_SOURCES
        external/quickjs-patched/libbf.c
        external/quickjs-patched/quickjs.c
        external/quickjs-patched/libregexp.c
        external/quickjs-patched/libunicode.c
        external/quickjs-patched/cutils.c
)

if ("${CMAKE_SYSTEM}" MATCHES "Linux" OR "${CMAKE_SYSTEM}" MATCHES "Darwin-*")
    find_package(OpenGL REQUIRED)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
endif ()

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

# Add the generated headers to your target's sources
target_sources(blipcade_cmake PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
)

# Make sure the binary dir is in the include path
target_include_directories(blipcade_cmake PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Link libraries
target_link_libraries(blipcade_cmake quickjs glfw ${OPENGL_LIBRARIES} nlohmann_json::nlohmann_json)

# Detect if building with Emscripten
if (DEFINED EMSCRIPTEN)
    set_target_properties(blipcade_cmake
            PROPERTIES SUFFIX ".html"
            LINK_FLAGS " --bind -s USE_GLFW=3 -s WASM=1 -g4 -s ALLOW_MEMORY_GROWTH=1 -s FULL_ES3=1 -s USE_WEBGL2=1")
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

# Add a custom target for documentation generation
add_custom_target(generate_js_docs DEPENDS ${CMAKE_BINARY_DIR}/js_api_docs.md)

# Add a custom target for TypeScript definitions
add_custom_target(generate_d_ts DEPENDS ${CMAKE_BINARY_DIR}/blipcade.d.ts)

# Make the main target depend on the documentation
add_dependencies(blipcade_cmake generate_js_docs generate_d_ts generate_data_header)