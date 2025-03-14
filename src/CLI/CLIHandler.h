/**
 * @file CLIHandler.h
 * @author Ollie
 * @brief Functions for CLI use
 * @version 1.0.0
 * @date 2025-03-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CLI_HANDLING_H
#define CLI_HANDLING_H

bool validateArguments(char* input_file, char* input_file_type, char* output_file, char* image_file, bool colour, char*& colour_type, bool totalFlow, bool watershed, int nPourPoints, char* process);

void printVerboseOutput(char* input_file, char* process, char* output_file, char* image_file, char* colour_type, bool verbose, bool watershed, int nPourPoints, char* watershed_directory, char* watershed_colour, bool totalFlow);

#endif