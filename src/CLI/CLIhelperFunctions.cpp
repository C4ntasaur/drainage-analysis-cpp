/**
 * @file CLIhelperFunctions.cpp
 * @author Ollie
 * @brief Helper functions for filenames, extensions, and checking integers for CLI user input
 * @see argumentParser.cpp
 * @version 1.0.0
 * @date 2025-03-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "CLIhelperFunctions.h"
#include <cstring>

/**
 * @brief Checks if a filename has a specific file extension.
 */
bool has_extension(const std::string& filename, const std::string& ext) {
    // Find the last '/' or '\' to get the base filename (ignoring directory path)
    size_t lastSlash = filename.find_last_of("/\\");
   
    std::string baseFilename;
    if (lastSlash == std::string::npos) {
        baseFilename = filename;
    } else {
        baseFilename = filename.substr(lastSlash + 1);
    }

    // Find last '.' in the base filename
    size_t dotPos = baseFilename.rfind('.');

    // If no '.' is found or is last character, no valid extension
    if (dotPos == std::string::npos || dotPos == baseFilename.size() - 1) {
        return false;  // No extension found
    }

    // Pull file extension and compare with given ext
    std::string fileExt = baseFilename.substr(dotPos + 1);
    return fileExt == ext;
}

/**
 * @brief Extracts the file extension from a given filename.
 */
std::string getFileExtension(const char* filename) {
    // Find the last occurrence of '/' or '\' to determine the start of the filename
    const char* lastSlash = strrchr(filename, '/');
    if (!lastSlash) {
        lastSlash = strrchr(filename, '\\');
    }

    // Extract filename after last slash
    const char* baseFilename;
    if (lastSlash) {
        // fileName starts after last slash
        baseFilename = lastSlash + 1;
    } else {
        // If no slash, use filename as fileName
        baseFilename = filename;
    }

    // Find the last '.' in the filename
    const char* dotPos = strrchr(baseFilename, '.');
    
    // If no '.' (or is last character), return an empty string
    if (!dotPos || dotPos == baseFilename + strlen(baseFilename) - 1) {
        return "";
    } else {
        return std::string(dotPos + 1);
    }
}


/**
 * @brief Checks if a given string represents a valid positive integer.
 */
bool isValidInteger(const char* str) {
    // Ensure the str is not null or empty
    if (str == nullptr || *str == '\0') {
        return false;
    }
    // Check each character to ensure it's a digit
    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isdigit(str[i])) {
            return false; // Non-digit character found, not a valid integer
        }
    }
    return true; // All characters are digits
}
