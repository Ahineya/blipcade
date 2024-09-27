cmake_minimum_required(VERSION 3.22)
project(blipcade_cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the directory containing your header files
include_directories(${CMAKE_SOURCE_DIR}/blipcade-renderer)
include_directories(${CMAKE_SOURCE_DIR}/blipcade-api)
include_directories(${CMAKE_SOURCE_DIR}/blipcade-runtime)
include_directories(${CMAKE_SOURCE_DIR}/external/glfw-3.2.1/include)
include_directories(${CMAKE_SOURCE_DIR}/external/quickjs-patched)


# Add your source files
set(SOURCES
        main.cpp
        blipcade-renderer/renderer.cpp
        blipcade-renderer/palette.cpp
        blipcade-renderer/color.cpp
        blipcade-renderer/font.cpp
        blipcade-renderer/spritesheet.cpp
        blipcade-renderer/canvas.cpp
        blipcade-api/converters.cpp
        blipcade-runtime/runtime.cpp
        blipcade-runtime/runtime.h
)

# Add QuickJS source files
set(QUICKJS_SOURCES
        external/quickjs-patched/libbf.c
        external/quickjs-patched/quickjs.c
        external/quickjs-patched/libregexp.c
        external/quickjs-patched/libunicode.c
        external/quickjs-patched/cutils.c
)

if("${CMAKE_SYSTEM}" MATCHES "Linux" OR "${CMAKE_SYSTEM}" MATCHES "Darwin-*")

    find_package(OpenGL REQUIRED)

    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw-3.2.1)
endif()


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
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(quickjs PRIVATE
            -Wno-unknown-warning-option
            -Wno-implicit-const-int-float-conversion
            -Wno-sign-conversion
    )
endif()

# Create the main executable target
add_executable(blipcade_cmake ${SOURCES})

# Add custom commands to generate shader headers
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/shader.vert
        ${CMAKE_CURRENT_BINARY_DIR}/shader_vert.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/shader.vert
        COMMENT "Generating vertex shader header"
)

add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/convert_shaders.py
        ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/shader.frag
        ${CMAKE_CURRENT_BINARY_DIR}/shader_frag.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/blipcade-renderer/shaders/shader.frag
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
target_link_libraries(blipcade_cmake quickjs glfw ${OPENGL_LIBRARIES})

# Detect if building with Emscripten
if (DEFINED EMSCRIPTEN)
    set_target_properties(blipcade_cmake
            PROPERTIES SUFFIX ".html"
            LINK_FLAGS " --bind -s USE_GLFW=3 -s WASM=1 -g4 -s ALLOW_MEMORY_GROWTH=1 -s FULL_ES3=1 -s USE_WEBGL2=1")
    message(STATUS "Configuring for Emscripten")
else()
    message(STATUS "Configuring for Desktop")
endif(DEFINED EMSCRIPTEN)
