/**
 * @file CLIHandler.cpp
 * @author Ollie
 * @brief Functions for CLI use
 * @version 1.0.0
 * @date 2025-03-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "CLIHandler.h"
#include <iostream>

bool validateArguments(char* input_file, char* input_file_type, char* output_file, char* image_file, bool colour, char*& colour_type, bool totalFlow, bool watershed, int nPourPoints, char* process) {
    if (!input_file) {
        std::cerr << "Error: No -i / --input flag provided." << std::endl;
        return false;
    }
    // No outputs
    if (!watershed && !output_file && !image_file) {
        std::cerr << "Error: At least one of -o (output file), -img (image file), or -w (watershed) must be specified." << std::endl;
        return false;
    }
    if (watershed) {
        if (output_file) {
            std::cerr << "Error: Watershed process is incompatible with output file (-o)." << std::endl;
            return false;
        }
        if (image_file) {
            std::cerr << "Error: Watershed process is incompatible with image file (-img)." << std::endl;
            return false;
        }
    }

    // Colour and image check
    if (image_file && !colour) {
        std::cout << "No -c flag. Greyscale chosen." << std::endl;
        if (colour_type == nullptr || strlen(colour_type) == 0) {
            colour_type = new char[3];
            strcpy(colour_type, "g1");
        }
    }

    // Total flow/watershed w/ process check
    if (totalFlow && !((strcmp(process, "mdf") == 0 || strcmp(process, "d8") == 0 || strcmp(process, "dinf") == 0 ))) {
            std::cerr << "Process " << process << " is not compatible with flow accumulation (-fa)." << std::endl;
            return false;
        }
        if (watershed && !((strcmp(process, "mdf") == 0 || strcmp(process, "d8") == 0 || strcmp(process, "dinf") == 0 ))) {
            std::cerr << "Process " << process << " is not compatible with watershed (-w)." << std::endl;
            return false;
        }
    
    // Watershed / flow accumulation clash
    if (totalFlow && watershed) {
        std::cerr << "Watershed and flow accumulation are incompatible processes." << std::endl;
            return false;
    }
    return true;
}

void printVerboseOutput(char* input_file, char* process, char* output_file, char* image_file, char* colour_type, bool verbose, bool watershed, int nPourPoints, char* watershed_directory, char* watershed_colour, bool totalFlow) {
    if (verbose) {
        std::cout << "Verbose mode enabled." << std::endl;
        std::cout << "Input file: " << input_file << std::endl;
        std::cout << "Process: " << process << std::endl;
        if (output_file) {
            std::cout << "Output file: " << output_file << std::endl;
        }
        if (image_file) {
            std::cout << "Image file: " << image_file << std::endl;
            if (colour_type) {
                std::cout << "Colour: " << colour_type << std::endl;
            }
        }
        if (watershed) {
            std::cout << "Watershed: Enabled" << std::endl;
            std::cout << "Watershed points: " << nPourPoints << std::endl;
            std::cout << "Watershed directory: " << watershed_directory << std::endl;
            std::cout << "Watershed colour: " << watershed_colour << std::endl;
        }
        else {
            std::cout << "Watershed: Disabled" << std::endl;
        }
        if (totalFlow) {
            std::cout << "Flow accumulation: Enabled" << std::endl;
        }
        else {
            std::cout << "Flow accumulation: Disabled" << std::endl;
        }
    }
}