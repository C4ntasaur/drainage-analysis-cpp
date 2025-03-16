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

/**
 * @brief Function to check inputs from CLI against each other
 * 
 * @param input_file full input file pathway of DEM
 * @param input_file_type type of file to be read (e.g. txt, csv)
 * @param output_file full output file pathway
 * @param image_file full image file pathway
 * @param colour if a colour has been specified for image out
 * @param colour_type colour short code from ../data/colourmaps/
 * @param totalFlow if flow accumulation was selected (-fa)
 * @param watershed if watershed delineation was chosen (-w x x x)
 * @param nPourPoints number of watershed delineation points chosen (-w 1 x x)
 * @param process type of process chosen (-p)
 * @return true 
 * @return false 
 */
bool validateArguments(char* input_file, char* input_file_type, char* output_file, char* image_file, bool colour, char*& colour_type, bool totalFlow, bool watershed, int nPourPoints, char* process);

/**
 * @brief Function to print verbose output
 * 
 * @param input_file full input file pathway of DEM
 * @param process  type of process chosen (-p)
 * @param output_file full output file pathway
 * @param image_file full image file pathway
 * @param colour_type colour short code from ../data/colourmaps/
 * @param verbose true or false (-v)
 * @param watershed if watershed delineation was chosen (-w x x x)
 * @param nPourPoints number of watershed delineation points chosen (-w 1 x x)
 * @param watershed_directory watershed delineation pathway for images (-w x /pathway/ x)
 * @param watershed_colour watershed delineation colourmap (-w x x colour)
 * @param totalFlow if flow accumation was selected (-fa)
 */
void printVerboseOutput(char* input_file, char* process, char* output_file, char* image_file, char* colour_type, bool verbose, bool watershed, int nPourPoints, char* watershed_directory, char* watershed_colour, bool totalFlow);

#endif