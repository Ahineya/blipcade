import argparse
import json

def main():
    parser = argparse.ArgumentParser(description='Builds blipcade cart from the cart description file.')
    parser.add_argument('path', help='Path to the description file')
    parser.add_argument('-o', '--output', help='Path to the output file.', default='blipcade-cart.json')

    args = parser.parse_args()

    with open(args.path, 'r') as file:
        cart = json.load(file)

        # Print the cart description
        # print(cart)

        # create a new json object
        cart_json = {
            "name": cart["name"],
        }

        if cart["code"]:
            with open(cart["code"], 'r') as code_file:
                cart_json["code"] = code_file.read()

        with open(args.output, 'w') as output_file:
            json.dump(cart_json, output_file)

if __name__ == '__main__':
    main()