project('quickjs', 'c',
        version : '2021.03.27',
        default_options : ['warning_level=3', 'c_std=c11'])

# Detect if we're using Emscripten
is_emscripten = meson.get_compiler('c').get_id() == 'emscripten'

# Source files
quickjs_sources = [
    'quickjs.c',
    'libregexp.c',
    'libunicode.c',
    'cutils.c',
    'quickjs-libc.c',
    'libbf.c',

]

# Include directories
inc = include_directories('.')

# Compiler flags
#c_args = ['-D_GNU_SOURCE', '-include', 'version.h']
c_args = ['-D_GNU_SOURCE']
if is_emscripten
    c_args += ['-s', 'WASM=1']
endif

# Build the library
if is_emscripten
    libquickjs = static_library('quickjs', quickjs_sources,
                                include_directories : inc,
                                c_args : c_args,
                                install : true)
else
    libquickjs = both_libraries('quickjs', quickjs_sources,
                                include_directories : inc,
                                c_args : c_args,
                                install : true)
endif

# Declare dependency
quickjs_dep = declare_dependency(
    include_directories : inc,
    link_with : libquickjs,
#    compile_args : ['-include', 'version.h']
)

# Install headers
install_headers(['quickjs.h', 'quickjs-libc.h'])

# Set variables to be retrieved by the main project
set_variable('quickjs_dep', quickjs_dep)
set_variable('quickjs_inc', inc)