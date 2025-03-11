#include <iostream>


bool writeBMP(const std::string& filename, const std::vector<unsigned char>& pixelData, int height, int width) {
    std::ofstream out(filename, std::ios::binary);

    if (!out.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    // Write header
    int rowSize = (width * 3 + 3) & ~3; // 4 byte aligned
    uint32_t dataSize = rowSize * height;

    BMPHeader bmpHeader = {0x4D42, 0, 0, 0, 0};
    DIBHeader dibHeader = {40, width, height, 1, 24, 0, dataSize, 0, 0, 0, 0};


    // Update file and image sizes
    bmpHeader.fileSize = 54 + dataSize;
    //dibHeader.imageSize = dataSize;

    out.write(reinterpret_cast<char*>(&bmpHeader), sizeof(bmpHeader));
    out.write(reinterpret_cast<char*>(&dibHeader), sizeof(dibHeader));

    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            out.put(pixelData[(i * width + j) * 3 + 2]); // R
            out.put(pixelData[(i * width + j) * 3 + 1]); // G
            out.put(pixelData[(i * width + j) * 3 + 0]); // B
        }

        int padding = rowSize - (width * 3);
        for (int p = 0; p < padding; p++) {
            out.put(0); // As a byte
        }
    }

    out.close();
    return true;

}