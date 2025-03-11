#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>

bool parseArguments(int argc, char* argv[], 
                     char*& input_file, 
                     char*& input_file_type,
                     char*& output_file, 
                     char*& image_file,
                     bool& colour,
                     char*& colour_type, 
                     bool& totalFlow,
                     bool& watershed,
                     int& nPourPoints,
                     bool& verbose, 
                     char*& process);

void printHelp();

#endif