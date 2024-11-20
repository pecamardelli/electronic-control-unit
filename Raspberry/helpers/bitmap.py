from PIL import Image, ImageDraw, ImageFont

# Set the image dimensions and colors
width, height = 36, 40  # Change these values as needed
background_color = (255, 0, 0)  # black background
text_color = (255, 255, 255)  # white color for the number

# Create a blank image with the specified background color
image = Image.new("RGB", (width, height), background_color)
draw = ImageDraw.Draw(image)

# Dynamically determine the best font size to fit the text within the image
text = "0"
font_size = 10  # Start with a small font size

# Increase font size until the text fits snugly within the image dimensions
while True:
    try:
        font = ImageFont.truetype("DejaVuSans-Bold.ttf", font_size)
    except IOError:
        font = ImageFont.load_default()
        break
    text_width, text_height = draw.textsize(text, font=font)
    if text_width >= width * 0.8 or text_height >= height * 0.8:
        break  # Stop if text is close to the full width or height
    font_size += 1

# Center the text within the image
text_x = (width - text_width) // 2
text_y = (height - text_height) // 2

# Draw the number onto the image
draw.text((text_x, text_y), text, font=font, fill=text_color)

# Convert the image to RGB565 format for Arduino
def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

byte_array = []
for y in range(height):
    for x in range(width):
        r, g, b = image.getpixel((x, y))
        rgb565 = rgb_to_rgb565(r, g, b)
        byte_array.append((rgb565 >> 8) & 0xFF)  # High byte
        byte_array.append(rgb565 & 0xFF)         # Low byte

# Convert the byte array to a C header file format in hexadecimal
hex_data = ', '.join(f'0x{byte:02X}' for byte in byte_array)
header_content = f"""#include "images.h"
const unsigned char number_{text}[{width * height*2}] = {{
    {hex_data}
}};
"""

# Save the content to a .h file for download
with open(f"number_{text}.cpp", "w") as file:
    file.write(header_content)

print(f"File 'number_{text}.cpp' generated successfully!")
