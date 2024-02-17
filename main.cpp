#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <jpeg_decoder.h>

void fill_buffer(uint8_t* buffer, size_t size) {
    for(size_t i = 0; i < size; i++) {
        buffer[i] = 0xFF;
    }
}

void decode_jpg_image(const std::string &input_file, uint8_t* &buffer, uint16_t& width, uint16_t& height) {
    size_t size;
    uint8_t* buf;
    FILE* f;

    f = fopen(input_file.c_str(), "rb");

    if (!f) {
        std::cerr << "Error opening " << input_file << std::endl;
        throw;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    buf = (unsigned char*)malloc(size);
    fseek(f, 0, SEEK_SET);
    size_t read = fread(buf, 1, size, f);
    fclose(f);

    Jpeg::Decoder decoder(buf, size);
    if (decoder.GetResult() != Jpeg::Decoder::OK) {
        std::cerr << "Error decoding " << input_file << std::endl;
        throw;
    }

    free(buf);

    width = decoder.GetWidth();
    height = decoder.GetHeight();
    uint8_t* image_bytes = decoder.GetImage();

    buffer = (uint8_t*)malloc(width * height / 8);

    fill_buffer(buffer, width * height / 8);

    if(decoder.IsColor()) {
        for(uint32_t i = 0; i < width * height; i++) {
            uint8_t r = image_bytes[i * 3];
            uint8_t g = image_bytes[i * 3 + 1];
            uint8_t b = image_bytes[i * 3 + 2];

            // value of all three of these should be less than 128 if it's black
            if(r < 128 && g < 128 && b < 128) {
                buffer[i / 8] &= ~(1 << (i % 8));
            }
        }
    } else {
        for(uint32_t i = 0; i < width * height; i++) {
            // if value is luminance then if it's less than 128 its probably black so make the pixel black
            if(image_bytes[i] < 128) {
                buffer[i / 8] &= ~(1 << (i % 8));
            }
        }
    }
}

void write_buffer_to_file(uint8_t* buffer, size_t size, std::ofstream& file) {
    file.write((char*)buffer, size);

    if(!file.good()) {
        std::cerr << "Error writing to file" << std::endl;
        throw;
    }
}

void read_file_and_fill_buffer(std::ifstream& file, uint8_t* buffer, uint16_t width) {
    std::string line;
    uint8_t to_write;
    uint16_t x, y;

    while(std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> x >> y;
        to_write = 1 << ((x % 8));
        x -= x % 8;

        buffer[(y * width/8) + (x / 8)] &= ~to_write;
    }
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file;
    uint16_t width, height;

    int opt;

    while((opt = getopt(argc, argv, "i:o:jw:b:hv")) != -1) {
        switch(opt) {
            case 'v':
                std::cout << "BIC" << std::endl;
                std::cout << "Binary image converter" << std::endl;
                std::cout << "Version: " << VERSION << std::endl;
                break;
            case 'h':
                std::cout << "Binary image converter is a handy cli tool that converts .css files" << std::endl;
                std::cout << "(exported from Aseprite as .css), or alternatevly a .jpg image" << std::endl;
                std::cout << "to a binary format suited for the GL library for the ESP32 paired with" << std::endl;
                std::cout << "the Sharp memory display." << std::endl;
                std::cout << "Please note, if the image is not purely black and white" << std::endl;
                std::cout << "the output will not be as you might expect." << std::endl;
                std::cout << "Usage: " << std::endl;
                std::cout << "-h Display this help message" << std::endl;
                std::cout << "-i <input_file> Specify the Aseprite exported .css input file if the '-j' flag is present it has to be a .jpg file" << std::endl;
                std::cout << "-o <output_file> Specify the name of the output file" << std::endl;
                std::cout << "-j If this flag is present the input file is a .jpg file make sure to supply this argument at the very end, '-w' and '-b' do not need to be supplyed when this flag is present" << std::endl;
                std::cout << "-w <image width> Specify the width of the image" << std::endl;
                std::cout << "-b <image height> Specify the height of the image" << std::endl;
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'j': {
                std::ofstream file(output_file, std::ios::out | std::ios::binary);
                uint8_t *buffer;

                decode_jpg_image(input_file, buffer, width, height);
                write_buffer_to_file(buffer, (width / 8) * height, file);
                free(buffer);
                return 0;
            }
            case 'w':
                width = atoi(optarg);
                break;
            case 'b':
                height = atoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " -i <input_file> -o <output_file> -w <image width> -b <image height>" << std::endl;
                return 1;
        }
    }

    std::ofstream file(output_file, std::ios::out | std::ios::binary);
    std::ifstream file_in(input_file, std::ios::in | std::ios::binary);
    uint8_t* image_buffer = new uint8_t[(width / 8) * height];
    fill_buffer(image_buffer, (width / 8) * height);

    if(!file) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    read_file_and_fill_buffer(file_in, image_buffer, width);

    write_buffer_to_file(image_buffer, (width / 8) * height, file);

    std::cout << "Finished converting file" << std::endl;

    return 0;
}
