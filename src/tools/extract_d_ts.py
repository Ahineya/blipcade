#!/usr/bin/env python3
# extract_d_ts.py

import re
import sys
import argparse
import os

def extract_jsdoc_comments(file_content):
    """
    Extracts all JSDoc comments from the given file content.

    Args:
        file_content (str): The content of the source file.

    Returns:
        list of str: A list containing all JSDoc comments.
    """
    pattern = re.compile(r'/\*\*([\s\S]*?)\*/', re.MULTILINE)
    matches = pattern.findall(file_content)
    cleaned_comments = [clean_jsdoc(comment) for comment in matches]
    return cleaned_comments

def clean_jsdoc(comment):
    """
    Cleans up a JSDoc comment by removing leading asterisks and whitespace.

    Args:
        comment (str): The raw JSDoc comment content.

    Returns:
        str: The cleaned JSDoc comment.
    """
    lines = comment.split('\n')
    cleaned_lines = []
    for line in lines:
        cleaned_line = re.sub(r'^\s*\* ?', '', line)
        cleaned_line = cleaned_line.replace('–', '-').replace('—', '-')
        cleaned_lines.append(cleaned_line)
    return '\n'.join(cleaned_lines).strip()

def parse_jsdoc(comments):
    """
    Parses JSDoc comments and organizes them by namespaces.

    Args:
        comments (list of str): List of cleaned JSDoc comments.

    Returns:
        dict: Organized data with namespaces containing functions and properties.
    """
    parsed_data = {}
    current_namespace = 'global'  # Default namespace

    for comment in comments:
        parsed = {
            'namespace': None,
            'function': None,
            'params': [],
            'returns': None,
            'description': [],
            'example': [],
            'properties': []
        }

        lines = comment.split('\n')
        for line in lines:
            if line.startswith('@namespace'):
                namespace = line[len('@namespace'):].strip()
                if namespace:
                    current_namespace = namespace
                    if current_namespace not in parsed_data:
                        parsed_data[current_namespace] = {'functions': [], 'properties': []}
                continue  # Move to next line after setting namespace

            elif line.startswith('@function'):
                func_name = line[len('@function'):].strip()
                if func_name:
                    parsed['function'] = func_name
                continue

            elif line.startswith('@param'):
                param = parse_param(line)
                if param:
                    parsed['params'].append(param)
                continue

            elif line.startswith('@returns') or line.startswith('@return'):
                ret = parse_return(line)
                if ret:
                    parsed['returns'] = ret
                continue

            elif line.startswith('@description'):
                description = line[len('@description'):].strip()
                parsed['description'].append(description)
                continue

            elif line.startswith('@example'):
                example = line[len('@example'):].strip()
                parsed['example'].append(example)
                continue

            elif line.startswith('@property'):
                prop = parse_property(line)
                if prop:
                    parsed['properties'].append(prop)
                continue

            else:
                # Handle multiline descriptions or additional lines
                if not any(line.startswith(tag) for tag in ['@namespace', '@function', '@param', '@returns', '@return', '@description', '@example', '@property']):
                    if line.strip():  # Ignore empty lines
                        parsed['description'].append(line.strip())

        # Assign parsed data to the current namespace
        if parsed['function'] or parsed['properties']:
            if current_namespace not in parsed_data:
                parsed_data[current_namespace] = {'functions': [], 'properties': []}
            if parsed['function']:
                parsed_data[current_namespace]['functions'].append(parsed)
            if parsed['properties']:
                parsed_data[current_namespace]['properties'].extend(parsed['properties'])

    return parsed_data

def parse_param(line):
    """
    Parses a @param line to extract parameter details.

    Args:
        line (str): The @param line.

    Returns:
        dict: Parameter details including name, type, description, optionality, and default value.
    """
    param_regex = re.compile(
        r'@param\s+'
        r'(?:\{([\w\[\]]+)\}\s+)?'                  # Optional {type}, supports array types like number[]
        r'(?:\[(\w+)(?:=([^\]]+))?\]|\b(\w+))'      # [name=default] or name
        r'(?:\s*-\s*)?'                             # Optional - separator
        r'(.*)'                                      # Description
    )
    match = param_regex.match(line)
    if match:
        param_type = match.group(1) if match.group(1) else 'any'
        param_name = match.group(2) if match.group(2) else match.group(4)
        param_default = match.group(3) if match.group(3) else None
        param_desc = match.group(5).strip() if match.group(5) else ''
        optional = bool(match.group(2))
        return {
            'name': param_name,
            'type': map_type(param_type),
            'description': param_desc,
            'optional': optional,
            'default': param_default
        }
    else:
        print(f"Warning: Unable to parse @param line: '{line}'")
        return None

def parse_return(line):
    """
    Parses a @returns or @return line to extract return type and description.

    Args:
        line (str): The @returns line.

    Returns:
        dict: Return type and description.
    """
    return_regex = re.compile(
        r'@returns?\s+'
        r'(?:\{([\w\[\]]+)\}\s+)?'  # Optional {type}
        r'(?:-\s*)?'
        r'(.*)'                      # Description
    )
    match = return_regex.match(line)
    if match:
        ret_type = match.group(1) if match.group(1) else 'void'
        ret_desc = match.group(2).strip()
        return {
            'type': map_type(ret_type),
            'description': ret_desc
        }
    else:
        print(f"Warning: Unable to parse @returns line: '{line}'")
        return None

def parse_property(line):
    """
    Parses a @property line to extract property details.

    Args:
        line (str): The @property line.

    Returns:
        dict: Property details including name, type, and description.
    """
    property_regex = re.compile(
        r'@property\s+\{([\w\[\]]+)\}\s+(\w+)\s*-\s*(.*)'
    )
    match = property_regex.match(line)
    if match:
        prop_type, prop_name, prop_desc = match.groups()
        return {
            'name': prop_name,
            'type': map_type(prop_type),
            'description': prop_desc.strip()
        }
    else:
        print(f"Warning: Unable to parse @property line: '{line}'")
        return None

def map_type(jsdoc_type):
    """
    Maps JSDoc types to TypeScript types.

    Args:
        jsdoc_type (str): The type specified in JSDoc.

    Returns:
        str: Corresponding TypeScript type.
    """
    type_mapping = {
        'string': 'string',
        'number': 'number',
        'boolean': 'boolean',
        'void': 'void',
        'any': 'any',
        'object': 'object',
        'array': 'any[]',
        'function': 'Function',
        'promise': 'Promise<any>',
        # Add more mappings as needed
    }
    # Handle array types like number[]
    array_match = re.match(r'(\w+)\[\]', jsdoc_type)
    if array_match:
        base_type = array_match.group(1)
        return f'{map_type(base_type)}[]'
    return type_mapping.get(jsdoc_type.lower(), 'any')

def generate_dts(parsed_data):
    """
    Generates TypeScript declaration content from parsed data.

    Args:
        parsed_data (dict): Organized data with namespaces containing functions and properties.

    Returns:
        str: The TypeScript declaration content.
    """
    dts_lines = []
    dts_lines.append('// Auto-generated TypeScript declarations\n')
    dts_lines.append('// Do not edit manually.\n\n')
    dts_lines.append('export {};\n\n')
    dts_lines.append('declare global {\n')

    # Handle global functions and properties
    if 'global' in parsed_data:
        for func in parsed_data['global']['functions']:
            if func['function']:
                dts_lines.append(generate_function_declaration(func, indent=4))
        for prop in parsed_data['global']['properties']:
            dts_lines.append(generate_property_declaration(prop, indent=4))

    # Handle nested namespaces
    for namespace, content in parsed_data.items():
        if namespace.lower() == 'global':
            continue  # Already handled
        dts_lines.append(f'    namespace {namespace} {{\n')
        for func in content['functions']:
            if func['function']:
                dts_lines.append(generate_function_declaration(func, indent=8))
        for prop in content['properties']:
            dts_lines.append(generate_property_declaration(prop, indent=8))
        dts_lines.append('    }\n\n')

    dts_lines.append('}\n')
    return ''.join(dts_lines)

def generate_function_declaration(func, indent=4):
    """
    Generates a TypeScript function declaration from parsed function info.

    Args:
        func (dict): Parsed function information.
        indent (int): Number of spaces for indentation.

    Returns:
        str: Function declaration in TypeScript.
    """
    indent_space = ' ' * indent
    func_name = func['function']
    params = func['params']
    return_type = func['returns']['type'] if func['returns'] else 'void'

    param_strings = []
    for param in params:
        optional = '?' if param['optional'] else ''
        param_strings.append(f'{param["name"]}{optional}: {param["type"]}')
    params_str = ', '.join(param_strings)

    # JSDoc for the function
    jsdoc = []
    if func['description']:
        jsdoc.append(f'{indent_space}/**\n')
        for desc_line in func['description']:
            jsdoc.append(f'{indent_space} * {desc_line}\n')
        jsdoc.append(f'{indent_space} */\n')

    declaration = f'{indent_space}function {func_name}({params_str}): {return_type};\n\n'
    return ''.join(jsdoc) + declaration

def generate_property_declaration(prop, indent=4):
    """
    Generates a TypeScript property declaration from parsed property info.

    Args:
        prop (dict): Parsed property information.
        indent (int): Number of spaces for indentation.

    Returns:
        str: Property declaration in TypeScript.
    """
    indent_space = ' ' * indent
    prop_name = prop['name']
    prop_type = prop['type']
    description = prop['description']
    jsdoc = []
    if description:
        jsdoc.append(f'{indent_space}/**\n{indent_space} * {description}\n{indent_space} */\n')
    declaration = f'{indent_space}const {prop_name}: {prop_type};\n\n'
    return ''.join(jsdoc) + declaration

def get_js_files(path):
    """
    Recursively retrieves all relevant files from the given directory.

    Args:
        path (str): The file or directory path.

    Returns:
        list of str: List of file paths.
    """
    valid_extensions = ('.js', '.cpp', '.ts', '.jsx', '.tsx')  # Extend as needed
    js_files = []
    if os.path.isfile(path):
        if path.endswith(valid_extensions):
            js_files.append(path)
    elif os.path.isdir(path):
        for root, _, files in os.walk(path):
            for file in files:
                if file.endswith(valid_extensions):
                    js_files.append(os.path.join(root, file))
    return js_files

def main():
    parser = argparse.ArgumentParser(description='Extract JSDoc comments from source files and convert them to TypeScript declarations.')
    parser.add_argument('path', help='Path to the source file or directory.')
    parser.add_argument('-o', '--output', help='Path to the output .d.ts file.', default='api.d.ts')

    args = parser.parse_args()

    source_files = get_js_files(args.path)

    if not source_files:
        print("No source files found in the specified path.")
        sys.exit(0)

    all_jsdoc_comments = []
    for file in source_files:
        try:
            with open(file, 'r', encoding='utf-8') as f:
                content = f.read()
            comments = extract_jsdoc_comments(content)
            if comments:
                all_jsdoc_comments.extend(comments)
        except Exception as e:
            print(f"Error processing file '{file}': {e}")

    if not all_jsdoc_comments:
        print("No JSDoc comments found in the specified files.")
        sys.exit(0)

    # Parse all comments and organize by namespaces
    parsed_data = parse_jsdoc(all_jsdoc_comments)

    # Generate TypeScript declarations
    dts_content = generate_dts(parsed_data)

    # Write to output file
    try:
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(dts_content)
        print(f"TypeScript declarations have been written to '{args.output}'.")
    except IOError as e:
        print(f"Error writing to file '{args.output}': {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()