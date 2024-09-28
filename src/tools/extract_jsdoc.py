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
    # Regular expression pattern to match JSDoc comments
    pattern = re.compile(r'/\*\*([\s\S]*?)\*/', re.MULTILINE)

    # Find all matches
    matches = pattern.findall(file_content)

    # Clean up the comments by stripping leading spaces and asterisks
    cleaned_comments = [clean_jsdoc(comment) for comment in matches]

    return cleaned_comments

def clean_jsdoc(comment):
    """
    Cleans up a JSDoc comment by removing leading asterisks and whitespace,
    and normalizing dash characters.

    Args:
        comment (str): The raw JSDoc comment content.

    Returns:
        str: The cleaned JSDoc comment.
    """
    # Split the comment into lines
    lines = comment.split('\n')
    cleaned_lines = []
    for line in lines:
        # Remove leading spaces and asterisks
        cleaned_line = re.sub(r'^\s*\* ?', '', line)
        # Replace en dashes and em dashes with hyphens
        cleaned_line = cleaned_line.replace('–', '-').replace('—', '-')
        cleaned_lines.append(cleaned_line)
    return '\n'.join(cleaned_lines).strip()

def parse_jsdoc(comment):
    """
    Parses a single JSDoc comment and converts it into Markdown format.

    Args:
        comment (str): The cleaned JSDoc comment.

    Returns:
        str: The Markdown representation of the JSDoc comment.
    """
    lines = comment.split('\n')
    markdown = []
    params = []
    properties = []
    description = []
    example = []
    returns = None
    throws = []
    function_name = ""
    brief = ""
    namespace = ""
    jsapi = False

    for line in lines:
        if line.startswith('@namespace'):
            namespace = line[len('@namespace'):].strip()
        elif line.startswith('@brief'):
            brief = line[len('@brief'):].strip()
        elif line.startswith('@function'):
            function_name = line[len('@function'):].strip()
        elif line.startswith('@property'):
            # Handle @property {type} name - description
            property_match = re.match(r'@property\s+\{(\w+)\}\s+(\w+)\s*-\s*(.*)', line)
            if property_match:
                prop_type, prop_name, prop_desc = property_match.groups()
                properties.append({
                    'type': prop_type,
                    'name': prop_name,
                    'description': prop_desc
                })
        elif line.startswith('@param'):
            # Existing param handling
            param_regex = re.compile(
                r'@param\s+'
                r'(?:\{(\w+)\}\s+)?'                  # Optional {type}
                r'(?:\[(\w+)(?:=([^\]]+))?\]|\b(\w+))'  # [name=default] or name
                r'(?:\s*-\s*)?'                       # Optional - separator
                r'(.*)'                                # Description
            )
            param_match = param_regex.match(line)
            if param_match:
                param_type = param_match.group(1) if param_match.group(1) else 'N/A'
                param_name = param_match.group(2) if param_match.group(2) else param_match.group(4)
                param_default = param_match.group(3) if param_match.group(3) else 'N/A'
                param_desc = param_match.group(5).strip()  # Corrected Line

                # Determine if the parameter is optional
                optional = bool(param_match.group(2))

                params.append({
                    'type': param_type,
                    'name': param_name,
                    'description': param_desc,
                    'optional': optional,
                    'default': param_default if optional else 'N/A'
                })
            else:
                print(f"Warning: Unable to parse @param line: '{line}'")
        elif line.startswith('@description'):
            description.append(line[len('@description'):].strip())
        elif line.startswith('@example'):
            example.append(line[len('@example'):].strip())
        elif line.startswith('@returns') or line.startswith('@return'):
            returns = line.split(' ', 1)[1].strip()
        elif line.startswith('@throws') or line.startswith('@exception'):
            throws_desc = line.split(' ', 1)[1].strip()
            throws.append(throws_desc)
        else:
            if not any(line.startswith(tag) for tag in ['@namespace', '@jsapi', '@brief', '@function', '@param', '@property', '@description', '@example', '@returns', '@return', '@throws', '@exception']):
                description.append(line.strip())

    if namespace:
        markdown.append(f'Namespace: `{namespace}`\n')
        markdown.append('---\n')
        if brief:
            markdown.append(f'**@brief** {brief}\n\n')
        if description:
            first_line = description[0]
            last_lines = description[1:]
            markdown.append(f'### {first_line}\n\n')
            markdown.append('\n'.join(last_lines) + '\n\n')

    if function_name:
        markdown.append(f'#### Function: `{function_name}`\n')
        if description:
            markdown.append(f'**Description:** { " ".join(description) }\n\n')
        if params:
            # Separate required and optional parameters
            required_params = [p for p in params if not p['optional']]
            optional_params = [p for p in params if p['optional']]

            if required_params:
                markdown.append('**Parameters (Required):**\n\n')
                markdown.append('| Name | Type | Description |\n')
                markdown.append('|------|------|-------------|\n')
                for param in required_params:
                    markdown.append(f'| `{param["name"]}` | `{param["type"]}` | {param["description"]} |\n')
                markdown.append('\n')

            if optional_params:
                markdown.append('**Parameters (Optional):**\n\n')
                markdown.append('| Name | Type | Default | Description |\n')
                markdown.append('|------|------|---------|-------------|\n')
                for param in optional_params:
                    markdown.append(f'| `{param["name"]}` | `{param["type"]}` | `{param["default"]}` | {param["description"]} |\n')
                markdown.append('\n')

        if returns:
            markdown.append(f'**Returns:** {returns}\n\n')

        if throws:
            markdown.append('**Throws:**\n')
            for throw in throws:
                markdown.append(f'- {throw}\n')
            markdown.append('\n')

        if example:
            markdown.append('**Example:**\n\n')
            markdown.append('```javascript\n')
            markdown.append('\n'.join(example) + '\n')
            markdown.append('```\n\n')

        markdown.append('---\n')

    if properties:
        markdown.append('**Properties:**\n\n')
        markdown.append('| Name | Type | Description |\n')
        markdown.append('|------|------|-------------|\n')
        for prop in properties:
            markdown.append(f'| `{prop["name"]}` | `{prop["type"]}` | {prop["description"]} |\n')
        markdown.append('\n')

    return ''.join(markdown)

def generate_markdown(jsdoc_comments):
    """
    Generates a Markdown document from a list of JSDoc comments, including a Table of Contents.

    Args:
        jsdoc_comments (list of str): The list of cleaned JSDoc comments.

    Returns:
        str: The complete Markdown document.
    """
    markdown = []
    toc = []
    function_sections = []

    for comment in jsdoc_comments:
        # Check if the comment is for a namespace to include in TOC
        if '@namespace' in comment:
            # Extract namespace name
            namespace_match = re.search(r'@namespace\s+`?(\w+)`?', comment)
            if namespace_match:
                namespace_name = namespace_match.group(1)
                toc.append(f'- [Namespace: {namespace_name}](#namespace-{namespace_name.lower()})')
        elif '@function' in comment:
            # Extract function name
            func_match = re.search(r'@function\s+`?(\w+)`?', comment)
            if func_match:
                func_name = func_match.group(1)
                toc.append(f'   - [Function: {func_name}](#function-{func_name.lower()})')
        elif '@property' in comment:
            # Extract property name
            prop_match = re.search(r'@property\s+\{(\w+)\}\s+(\w+)\s*-\s*(.*)', comment)
            if prop_match:
                prop_type, prop_name, prop_desc = prop_match.groups()
                toc.append(f'   - [Property: {prop_name}](#property-{prop_name.lower()})')

        function_section = parse_jsdoc(comment)
        function_sections.append(function_section)

    # Add Table of Contents
    markdown.append('# Blipcade JavaScript API Documentation\n\n')
    if toc:
        markdown.append('## Table of Contents\n\n')
        markdown.extend([line + '\n' for line in toc])
        markdown.append('\n---\n\n')

    # Add all sections
    markdown.extend(function_sections)

    return ''.join(markdown)

def get_js_files(path):
    """
    Recursively retrieves all JavaScript and relevant files from the given directory.

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
    # Set up command-line argument parsing
    parser = argparse.ArgumentParser(description='Extract JSDoc comments from source files and convert them to Markdown.')
    parser.add_argument('path', help='Path to the source file or directory.')
    parser.add_argument('-o', '--output', help='Path to the output Markdown file.', default='jsdoc.md')

    args = parser.parse_args()

    # Get list of source files
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

    # Generate Markdown
    markdown_content = generate_markdown(all_jsdoc_comments)

    # Write to the output file
    try:
        with open(args.output, 'w', encoding='utf-8') as f:
            f.write(markdown_content)
        print(f"Markdown documentation has been written to '{args.output}'.")
    except IOError as e:
        print(f"Error writing to file '{args.output}': {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()