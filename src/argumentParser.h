/**
 * @file argumentParser.h
 * @author Ollie
 * @brief Argument parser for CLI user inputs
 * @version 1.0.0
 * @date 2025-03-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <string>

/**
 * @brief Method that checks user inputs for CLI
 * 
 * @param argc Number of arguments given with ./drainage-analysis
 * @param argv Array of arguments given with ./drainage-analysis
 * @param input_file Input file argument
 * @param input_file_type File extension of input file
 * @param output_file Output file argument
 * @param image_file File extension of output file
 * @param colour Bool for if colour was chosen
 * @param colour_type User colour choice 
 * @param totalFlow Bool for flow accumulation algorithms
 * @param watershed Bool for watershed delineation algorithms
 * @param nPourPoints Number of pour points specified by user
 * @param verbose Verbose mode for CLI
 * @param process Process specified by user
 * @return true If arguments given by user were valid
 * @return false Otherwise
 */
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

/**
 * @brief Quick print help function
 */
void printHelp();

#endif