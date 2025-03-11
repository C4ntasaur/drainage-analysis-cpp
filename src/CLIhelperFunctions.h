#ifndef CLIHELPERFUNCTIONS_H
#define CLIHELPERFUNCTIONS_H

#include <string>

bool has_extension(const std::string& filename, const std::string& ext);
std::string getFileExtension(const char* filename);
bool isValidInteger(const char* str);

#endif