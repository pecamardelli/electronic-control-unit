import os
from PIL import Image, ImageDraw, ImageFont
import string

print(f"Current working directory: {os.getcwd()}")

# Convert the image to RGB565 format
def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def calculate_max_font_size(text, width, height, font_name):
    # Create a blank image
    image = Image.new("RGB", (width, height), (0, 0, 0))
    draw = ImageDraw.Draw(image)

    font_size = 10  # Start with a small font size
    max_font_size = 10  # To track the maximum font size that fits all characters
    characters = list(text)  # Split text into individual characters

    # Loop to find the maximum font size that fits all characters
    while True:
        try:
            font = ImageFont.truetype(font_name, font_size)
        except IOError:
            font = ImageFont.load_default()
            break

        fits = True  # Flag to check if the current font size works for all characters
        for char in characters:
            bbox = draw.textbbox((0, 0), char, font=font)
            text_width = bbox[2] - bbox[0]  # right - left
            text_height = bbox[3] - bbox[1]  # bottom - top
            if text_width >= width * 0.9 or text_height >= height * 0.9:
                fits = False  # This font size doesn't fit the current character
                break
        
        if not fits:
            break  # Stop if any character exceeds the dimensions

        max_font_size = font_size  # Update the maximum font size
        font_size += 1
    
    return max_font_size

def calculate_max_character_size(text, font_name, font_size):
    # Load the font
    try:
        font = ImageFont.truetype(font_name, font_size)
    except IOError:
        font = ImageFont.load_default()
    
    # Create a dummy drawing context
    dummy_image = Image.new("RGB", (1, 1))  # Minimal size for dummy context
    draw = ImageDraw.Draw(dummy_image)
    
    # Variables to track the maximum width and height
    max_width = 0
    max_height = 0

    # Iterate over each character in the text
    for char in text:
        bbox = draw.textbbox((0, 0), char, font=font)
        char_width = bbox[2] - bbox[0]  # right - left
        char_height = bbox[3] - bbox[1]  # bottom - top

        # Update max dimensions
        max_width = max(max_width, char_width)
        max_height = max(max_height, char_height)
    
    return max_width, max_height  # Return the largest width and height

def get_hex_data(text):
    # Create a blank image with the specified background color
    image = Image.new("RGB", (width, height), background_color)
    draw = ImageDraw.Draw(image)

    # Draw the number onto the image
    draw.text((0, 0), text, font=font, fill=text_color)

    byte_array = []
    for y in range(height):
        for x in range(width):
            r, g, b = image.getpixel((x, y))
            rgb565 = rgb_to_rgb565(r, g, b)
            byte_array.append((rgb565 >> 8) & 0xFF)  # High byte
            byte_array.append(rgb565 & 0xFF)         # Low byte

    # Convert the byte array to a C header file format in hexadecimal
    return ', '.join(f'0x{byte:02X}' for byte in byte_array)


# Set the image dimensions and colors
font_name = "LiberationSansNarrow-Bold.ttf"
# font_name = f"{os.getcwd()}/assets/fonts/BebasNeue-Regular.ttf"
# font_size = calculate_max_font_size(string.ascii_uppercase,width=width,height=height,font_name=font_name)
font_size = 60
font = ImageFont.truetype(font_name, font_size)
width, height = calculate_max_character_size(string.printable,font_name=font_name,font_size=font_size)
width -= 15
height += 5
print(width,height)
path = f"{os.getcwd()}/assets/chars"
header_file_name = "characters"
header_file_content = f"""// Font: {font_name}

#ifndef _{header_file_name.upper()}_H_
#define _{header_file_name.upper()}_H_

#include <cstdint>\n\n"""

header_struct_content = []

background_color = (0, 0, 0)  # black background
text_color = (255, 255, 255)  # white color for the number
file_name = "punctuation_"

# Lowercase letters
for index,char in enumerate("!\"#$%&'()*+,-./"):
    hex_data = get_hex_data(text=char)

    char_cpp_file_content = f"""#include "{header_file_name}.h"
const unsigned char {file_name}{index}[{width * height*2}] = {{
    {hex_data}
}};"""

    header_file_content += f"extern const unsigned char {file_name}{index}[];\n"
    header_struct_content.append(f"{file_name}{index}")

    # Save the content to a .h file for download
    with open(f"{path}/{file_name}{index}.cpp", "w") as file:
        file.write(char_cpp_file_content)

    print(f"File '{file_name}{index}.cpp' generated successfully!")

file_name = "char_"

# Lowercase letters
for char in string.digits + string.ascii_uppercase + string.ascii_lowercase:
    hex_data = get_hex_data(text=char)

    char_cpp_file_content = f"""#include "{header_file_name}.h"
const unsigned char {file_name}{char}[{width * height*2}] = {{
    {hex_data}
}};"""

    header_file_content += f"extern const unsigned char {file_name}{char}[];\n"
    header_struct_content.append(f"{file_name}{char}")

    # Save the content to a .h file for download
    with open(f"{path}/{file_name}{char}.cpp", "w") as file:
        file.write(char_cpp_file_content)

    print(f"File '{file_name}{char}.cpp' generated successfully!")

header_file_content += f"""\n
struct _{header_file_name}
{{
    const uint16_t width;
    const uint16_t height;
    const unsigned char *characters[];
}};

extern const _{header_file_name} {header_file_name};

#endif"""

struct_cpp_file_content = f"""#include "{header_file_name}.h"\n
const _{header_file_name} {header_file_name} = {{{width}, {height}, {{{', '.join(f'{char_name}' for char_name in header_struct_content)}}}}};"""

# Finally, save the header file
with open(f"{path}/{header_file_name}.cpp", "w") as file:
    file.write(struct_cpp_file_content)

# Finally, save the header file
with open(f"{path}/{header_file_name}.h", "w") as file:
    file.write(header_file_content)
