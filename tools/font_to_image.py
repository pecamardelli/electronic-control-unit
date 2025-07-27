import os
import string
from PIL import Image, ImageDraw, ImageFont
from datetime import datetime

print(f"Current working directory: {os.getcwd()}")

# Convert the image to RGB565 format
def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

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
    image = Image.new("1", (width, height), 0)  # Background is black (0)
    draw = ImageDraw.Draw(image)

    # Render the text onto the image
    draw.text((0, 0), text, font=font, fill=1)  # Text is white (1)

        # Extract pixels and convert to an 8-bit array
    pixels = image.load()
    byte_array = []
    
    for y in range(height):
        byte = 0
        for x in range(width):
            # Shift the bit to the left and set it based on the pixel value
            byte = (byte << 1) | pixels[x, y]
            if x % 8 == 7:  # If 8 bits are collected, append the byte
                byte_array.append(byte)
                byte = 0
        if width % 8 != 0:  # Handle leftover bits at the end of the row
            byte <<= (8 - width % 8)  # Pad the remaining bits with 0
            byte_array.append(byte)
    
    return ', '.join(f'0x{byte:02X}' for byte in byte_array)


# Set the image dimensions and colors
font_file = "LiberationSansNarrow-Bold.ttf"
font_name, extension = os.path.splitext(font_file)
font_name = font_name.replace("-","_")
# font_name = f"{os.getcwd()}/assets/fonts/BebasNeue-Regular.ttf"
# font_size = calculate_max_font_size(string.ascii_uppercase,width=width,height=height,font_name=font_name)
# font_size = 60
font_sizes = [28,30]
path = f"{os.getcwd()}/tools"
background_color = (0, 0, 0)  # black background
text_color = (255, 255, 255)  # white color for the number

characters = " !\"#$%&'()*+,-./" + string.digits + ":;<=>?@" + string.ascii_uppercase + "[\\]^_`"+ string.ascii_lowercase + "{|}~"

current_date = datetime.now()
# Format the date
formatted_date = current_date.strftime("%d-%B-%Y")

for font_size in font_sizes:
    font = ImageFont.truetype(font_file, font_size)
    width, height = calculate_max_character_size(string.printable,font_name=font_file,font_size=font_size)
    width = round(width*0.70)
    c_file_name = f"{font_name}{font_size}.c"
    c_file_content = f"""/**
******************************************************************************
* @file    {c_file_name}
* @author  MCD Application Team
* @version V1.0.0
* @date    {formatted_date}
* @brief   This file provides text {font_name}{font_size} for STM32xx-EVAL's LCD driver. 
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "fonts.h"

// 
//  Font data for {font_name} {font_size}pt
// 

const uint8_t {font_name}{font_size}_Table[] = 
{{
"""

    # All characters
    for char in characters:
        c_file_content += get_hex_data(text=char)
        c_file_content += ",\n"

    c_file_content += f"""}};

sFONT {font_name}{font_size} = {{
{font_name}{font_size}_Table,
{width}, /* Width */
{height}, /* Height */
}};

/************************ (C) COPYRIGHT Pablin *****END OF FILE****/"""

    with open(f"{path}/{c_file_name}", "w") as file:
        file.write(c_file_content)

    print(f"File '{c_file_name}' generated successfully!")
