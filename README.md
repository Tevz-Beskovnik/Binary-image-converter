# BINARY IMAGE CONVERTER

Binary image converter is a handy cli tool that converts .css files 
(exported from Aseprite as .css), or alternatevly a .jpg image to a 
binary format suited for the GL (Link coming soon) library for the ESP32 paired with the 
Sharp memory display. 

Please note, if the image is not purely black and 
white the output will not be as you might expect.

## Installation

All you need is gcc and cmake. Build with provided cmake
file.

----
Thanks to Scott Graham for the jpeg_decore.h [library](https://scot.tg/2009/12/02/mini-jpeg-decoder/)