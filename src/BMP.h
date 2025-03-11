/*
Header file for a very simple, write-only bitmap creator.
This implementation only works with 24-bit output meaning only RGB and not RGBA. This was chosen for its simplicity and wider use over 32-bit.

Have used typedef for BYTE, DWORD, etc. as this is how I learnt memory sizes from CS50. I know it is not typical.

Structs were inspired by wingdi.h header by Microsoft https://learn.microsoft.com/en-gb/windows/win32/api/wingdi/.
Headers give the correct binary preamble to ensure the file is recognised as a .bmp before any image data is read.


*/

#ifndef BMP_H
#define BMP_H

#include <fstream>
#include <iostream>
#include <vector>

#pragma pack(push, 1)

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

struct BITMAPFILEHEADER {
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
};

struct BITMAPINFOHEADER{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
};

struct RGBTRIPLE {
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
};

#pragma pack(pop)


class BMP {
public:
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    RGBTRIPLE rgb;
    std::vector<RGBTRIPLE> data;

    // Constructor    
    BMP(int32_t width, int32_t height) {
        int rowSize = (width * 3 + 3) & ~3; // Row size (padded to 4 bytes)
        int imageSize = rowSize * height;

        // File Header (14 bytes)
        FileHeader.bfType = 0x4D42;  // "BM" in ASCII
        FileHeader.bfSize = 54 + imageSize; // Total file size
        FileHeader.bfReserved1 = 0;
        FileHeader.bfReserved2 = 0;
        FileHeader.bfOffBits = 54; // Pixel data starts after headers

        // Info Header (40 bytes)
        InfoHeader.biSize = 40; // Header size
        InfoHeader.biWidth = width;
        InfoHeader.biHeight = height;
        InfoHeader.biPlanes = 1;
        InfoHeader.biBitCount = 24; // 24-bit BMP
        InfoHeader.biCompression = 0; // No compression
        InfoHeader.biSizeImage = imageSize; // Image data size
        InfoHeader.biXPelsPerMeter = 2835; // 72 DPI (2835 pixels per meter)
        InfoHeader.biYPelsPerMeter = 2835; // 72 DPI
        InfoHeader.biClrUsed = 0; // No color palette for 24-bit
        InfoHeader.biClrImportant = 0;

        data.resize(width * height);  // Resize the vector to hold all pixels
        for (auto& pixel : data) {
            pixel = {0, 0, 0}; // Fill with black pixels (BGR format)
        }
    }

    void write(const char* filename) {
        std::ofstream outFile(filename, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Could not open image file: " << filename << std::endl;
            return;
        }

        outFile.write(reinterpret_cast<char*>(&FileHeader), sizeof(FileHeader));
        outFile.write(reinterpret_cast<char*>(&InfoHeader), sizeof(InfoHeader));

        int rowSize = (InfoHeader.biWidth * 3 + 3) & ~3;
        int padding = rowSize - InfoHeader.biWidth * 3;

        BYTE paddingBytes[3] = {0, 0, 0};

        for (int i = 0; i < InfoHeader.biHeight; i++) {
            for (int j = 0; j < InfoHeader.biWidth; j++) {
                RGBTRIPLE pixel = data[(i * InfoHeader.biWidth) + j];
                outFile.write(reinterpret_cast<char*>(&pixel), sizeof(pixel));
            }
            if (padding > 0) {
                outFile.write(reinterpret_cast<char*>(paddingBytes), padding);
            }
        }
        outFile.close();
    }

    void setPixel(int x, int y, RGBTRIPLE pixel) {
        if (x >= 0 && x < InfoHeader.biWidth && y >= 0 && y < InfoHeader.biHeight) {
            data[(y * InfoHeader.biWidth) + x] = pixel;
        } else {
            std::cerr << "Error: Pixel out of bounds (" << x << ", " << y << ")\n";
        }
    }
    
};

#endif