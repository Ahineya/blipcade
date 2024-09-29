import os
import sys

def glsl_to_cpp_header(input_file, output_file):
    with open(input_file, 'r') as glsl_file:
        glsl_code = glsl_file.read()

    with open(output_file, 'w') as cpp_file:
        shader_name = os.path.splitext(os.path.basename(input_file))[0]
        ext = os.path.splitext(input_file)[1]
        shader_name = shader_name + '_fragment' if ext == '.frag' else shader_name + '_vertex'

        cpp_file.write(f"#ifndef {shader_name.upper()}_H\n")
        cpp_file.write(f"#define {shader_name.upper()}_H\n\n")

        cpp_file.write("#if defined(EMSCRIPTEN)\n")
        cpp_file.write(f"const char* {shader_name}Source = R\"glsl(")
        cpp_file.write("#version 300 es\n")
        if ext == ".frag":
            cpp_file.write("precision mediump float;\n")
        cpp_file.write(glsl_code)
        cpp_file.write(")glsl\";\n")
        cpp_file.write("#else\n")
        cpp_file.write(f"const char* {shader_name}Source = R\"glsl(\n")
        cpp_file.write("#version 410\n")
        cpp_file.write(glsl_code)
        cpp_file.write(")glsl\";\n")
        cpp_file.write("#endif\n\n")

        cpp_file.write("#endif\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_shaders.py <input_glsl_file> <output_cpp_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    glsl_to_cpp_header(input_file, output_file)