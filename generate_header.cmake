# generate_header.cmake

# Ensure required variables are defined
if(NOT DEFINED JSON_FILE)
    message(FATAL_ERROR "JSON_FILE variable is not defined.")
endif()

if(NOT DEFINED HEADER_FILE)
    message(FATAL_ERROR "HEADER_FILE variable is not defined.")
endif()

# Read the JSON file content
file(READ "${JSON_FILE}" JSON_CONTENT)

# Open the header file for writing
file(WRITE "${HEADER_FILE}" "#ifndef JSON_CART_HPP\n")
file(APPEND "${HEADER_FILE}" "#define JSON_CART_HPP\n\n")
file(APPEND "${HEADER_FILE}" "#include <string>\n\n")
file(APPEND "${HEADER_FILE}" "constexpr const char* json_cart_data = R\"DELIM(\n")
file(APPEND "${HEADER_FILE}" "${JSON_CONTENT}\n")
file(APPEND "${HEADER_FILE}" ")DELIM\";\n\n")
file(APPEND "${HEADER_FILE}" "#endif // JSON_CART_HPP\n")