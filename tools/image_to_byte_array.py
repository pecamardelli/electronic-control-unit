import os
from PIL import Image

print(f"Current working directory: {os.getcwd()}")

path = f"{os.getcwd()}/"
header_file_name = "torino_logo_sm"
header_file_content = f"""#ifndef _{header_file_name.upper()}_H_
#define _{header_file_name.upper()}_H_\n\n"""

# Define image extensions
image_extensions = {".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff"}

# Loop through the directory
for filenamefull in os.listdir(path):
    filename = os.path.splitext(filenamefull)[0].lower()
    extension = os.path.splitext(filenamefull)[1].lower()
    print(filenamefull)
    # Get the file extension and check if it's an image
    if extension in image_extensions:
        image_path = f"{path}/{filenamefull}"
        image = Image.open(image_path).convert("RGB")
        width, height = image.size

        # Convert the image to a 16-bit RGB565 format
        def rgb_to_rgb565(r, g, b):
            return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

        # Create the byte array
        byte_array = []
        for y in range(height):
            for x in range(width):
                r, g, b = image.getpixel((x, y))
                rgb565 = rgb_to_rgb565(r, g, b)
                byte_array.append((rgb565 >> 8) & 0xFF)  # High byte
                byte_array.append(rgb565 & 0xFF)         # Low byte

        # Format the byte array as a C++ header file
        hex_data = ', '.join(f'0x{byte:02X}' for byte in byte_array)
        header_content = f"""#include "images.h"

        const unsigned char {filename}[{width*height*2}] = {{
            {hex_data}
        }};
        """

        header_file_content += f"extern const unsigned char {filename}[];\n"

        # Save to a .cpp file
        with open(f"{path}/{filename}.cpp", "w") as file:
            file.write(header_content)

        print(f"File '{filename}.cpp' generated successfully!")


header_file_content += "\n#endif"

# Finally, save the header file
with open(f"{path}/{header_file_name}.h", "w") as file:
    file.write(header_file_content)
