#include "CLIhelperFunctions.h"
#include <cstring>

bool has_extension(const std::string& filename, const std::string& ext) {
    size_t lastSlash = filename.find_last_of("/\\");
    std::string baseFilename = (lastSlash == std::string::npos) ? filename : filename.substr(lastSlash + 1);
    size_t dotPos = baseFilename.rfind('.');
    if (dotPos == std::string::npos || dotPos == baseFilename.size() - 1) {
        return false;
    }
    std::string fileExt = baseFilename.substr(dotPos + 1);
    return fileExt == ext;
}

std::string getFileExtension(const char* filename) {
    const char* lastSlash = strrchr(filename, '/');
    if (!lastSlash) {
        lastSlash = strrchr(filename, '\\');
    }
    const char* fileName = lastSlash ? lastSlash + 1 : filename;
    const char* dotPos = strrchr(fileName, '.');
    if (!dotPos || dotPos == fileName + strlen(fileName) - 1) {
        return "";
    } else {
        return std::string(dotPos + 1);
    }
}

bool isValidInteger(const char* str) {
    if (str == nullptr || *str == '\0') return false;

    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isdigit(str[i])) return false;
    }
    return true;
}
