#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image {
    public:
        rtw_image() {}
        rtw_image(const char* image_filename) {
            if (load(image_filename))
                return;
            std::cerr << "Could not load image file " << image_filename << std::endl;
        }

        ~rtw_image() {
            delete[] bdata;
            stbi_image_free(fdata);
        }

        bool load(const std::string& filename) {
            auto n = bytes_per_pixel;
            fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
            if (fdata == nullptr) return false;

            bytes_per_scanline = image_width * bytes_per_pixel;
            convert_to_bytes();
            
            return true;
        }

        int width() const { return (fdata == nullptr) ? 0 : image_width; }
        int height() const { return (fdata == nullptr) ? 0 : image_height; }

        const unsigned char* pixel_data(int x, int y) const {
            static unsigned char magenta[] = {255, 0, 255}; // For debugging
            if (bdata == nullptr) 
                return magenta;

            x = clamp(x, 0, image_width);
            y = clamp(y, 0, image_height);

            return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
        }
    
    private:
        const int bytes_per_pixel = 3;
        float *fdata = nullptr;
        unsigned char *bdata = nullptr;
        int image_width = 0;
        int image_height = 0;
        int bytes_per_scanline = 0;

        static int clamp(int x, int low, int high) {
            // Returns x clamped to [low, high}
            if (x < low) return low;
            if (x < high) return x;
            return high - 1;
        }

        static unsigned char float_to_byte(float value) {
            if (value <= 0.0)
                return 0;
            if (1.0 <= value)
                return 255;
            return static_cast<unsigned char>(256.0 * value);
        }

        void convert_to_bytes() {
            // Converts float data at fptr to byte data stored at bptr
            int total_bytes = image_width * image_height * bytes_per_pixel;
            bdata = new unsigned char[total_bytes];

            unsigned char *bptr = bdata;
            float *fptr = fdata;
            for (int i = 0; i < total_bytes; i++, bptr++, fptr++)
                *bptr = float_to_byte(*fptr);
        }
};

#endif