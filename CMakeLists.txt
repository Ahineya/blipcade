cmake_minimum_required(VERSION 3.22)
project(blipcade_cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the directory containing your header files
include_directories(${CMAKE_SOURCE_DIR}/blipcade-renderer)
include_directories(${CMAKE_SOURCE_DIR}/external/SDL2/include)
include_directories(${CMAKE_SOURCE_DIR}/external/quickjs)

# Add your source files
set(SOURCES
        main.cpp
        blipcade-renderer/renderer.cpp
        blipcade-renderer/palette.cpp
        blipcade-renderer/color.cpp
)

# Add QuickJS source files
set(QUICKJS_SOURCES
        external/quickjs/libbf.c
        external/quickjs/quickjs.c
        external/quickjs/libregexp.c
        external/quickjs/libunicode.c
        external/quickjs/cutils.c
)

# Read QuickJS version from the version file
file(READ "${CMAKE_SOURCE_DIR}/external/quickjs/VERSION" QUICKJS_VERSION)
string(STRIP "${QUICKJS_VERSION}" QUICKJS_VERSION)

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

add_executable(blipcade_cmake ${SOURCES})

# Link QuickJS
target_link_libraries(blipcade_cmake PRIVATE quickjs)

# Detect if building with Emscripten
if (EMSCRIPTEN)
    message(STATUS "Configuring for Emscripten")
    set_target_properties(blipcade_cmake PROPERTIES
            SUFFIX ".html"
            LINK_FLAGS "-s WASM=1 -s USE_GLFW=3 -s USE_SDL=2 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] --shell-file ${CMAKE_SOURCE_DIR}/shell.html"
    )
else()
    message(STATUS "Configuring for Desktop")
    # Find SDL2 package
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(blipcade_cmake PRIVATE ${SDL2_LIBRARIES})
endif()

# Add math library for non-Emscripten builds
if(NOT EMSCRIPTEN)
    target_link_libraries(blipcade_cmake PRIVATE m)
endif()