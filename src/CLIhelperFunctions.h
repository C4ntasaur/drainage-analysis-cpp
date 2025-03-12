/**
 * @file CLIhelperFunctions.h
 * @author Ollie
 * @brief Helper functions for string and integer checking used in UI
 * @version 1.0.0
 * @date 2025-03-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CLIHELPERFUNCTIONS_H
#define CLIHELPERFUNCTIONS_H

#include <string>

/**
 * @brief Checks if a filename has a specific file extension.
 * 
 * This function is case-sensitive. Example:
 * - "temp.txt", "txt" -> true
 * - "temp.TXT", "txt" -> false
 * 
 * @param filename Filename to check.
 * @param ext Expected file extension (without '.').
 * @return true If the filename has the specified extension.
 * @return false Otherwise.
 */
bool has_extension(const std::string& filename, const std::string& ext);

/**
 * @brief Extracts the file extension from a given filename.
 * 
 * Returns the substring after the last `.` in the filename.
 * If no extension is found, or if the filename ends with `.`, an empty string is returned.
 * 
 * @param filename Input filename.
 * @return std::string Extracted file extension or an empty string if none exists.
 */
std::string getFileExtension(const char* filename);

/**
 * @brief Checks if a given string represents a valid positive integer.
 * 
 * Only digits (0-9) are considered valid. Signs (+, -) are not supported.
 * 
 * @param str Input string.
 * @return true If the string is a valid positive integer.
 * @return false Otherwise.
 */
bool isValidInteger(const char* str);

#endif