#include "argumentParser.h"
#include "CLIhelperFunctions.h"
#include <iostream>
#include <cstring>

void printHelp() {
    std::cout << "Help: Use the following flags:" << std::endl;
    std::cout << "-i <input_file> : Specify input file (.csv, .txt, .bin)" << std::endl;
    std::cout << "-p <process> : Specify the process to apply (e.g., 'flow', 'slope')" << std::endl;
    std::cout << "-fa <flow_accumulation> : If selected will run flow accumulation analysis" << std::endl;
    std::cout << "-w <watershed> : If selected will run watershed analysis" << std::endl;
    std::cout << "-o <output_file> : Specify output file (.txt, .csv, .bin)" << std::endl;
    std::cout << "-img <image_file> : Specify output image (.bmp)" << std::endl;
    std::cout << "-c <colour> : Specify colour palette for image output" << std::endl;
    std::cout << "-v, --verbose : Enable verbose output" << std::endl;
}

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
                     char*& process) {
    if (argc < 2) {
        std::cout << "No flags provided. Use -h for help." << std::endl;
        return false;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
            return false;
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 < argc) {
                // Free previously allocated memory if it exists
                if (input_file) {
                    delete[] input_file;
                }
        
                // Allocate memory for input file and check for existence
                input_file = new char[strlen(argv[i + 1]) + 1];
                if (!input_file) {
                    std::cerr << "Error: Memory allocation failed for input_file." << std::endl;
                    return false;
                }
                strcpy(input_file, argv[i + 1]);
        
                // Validate file extension
                if (!has_extension(input_file, "csv") && 
                    !has_extension(input_file, "txt") && 
                    !has_extension(input_file, "bin")) {
                    std::cerr << "Error: Invalid input file extension. Supported extensions are .csv, .txt, .bin." << std::endl;
                    delete[] input_file;
                    input_file = nullptr; // Prevent open pointer
                    return false;
                }
        
                // Extract and store file extension
                std::string ext = getFileExtension(input_file);
        
                // Free previously allocated memory for input_file_type
                if (input_file_type) {
                    delete[] input_file_type;
                }
        
                input_file_type = new char[ext.length() + 1];
                if (!input_file_type) {
                    std::cerr << "Error: Memory allocation failed for input_file_type." << std::endl;
                    delete[] input_file; // Delete allocated string before failure
                    input_file = nullptr;
                    return false;
                }
                strcpy(input_file_type, ext.c_str());
        
                i++;  // Skip the next argument (filename)
            } else {
                std::cerr << "Error: -i flag requires an input filename." << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--process") == 0) {
            if (process != nullptr) {
                std::cerr << "Error: Only one process can be specified." << std::endl;
                return false;
            }
            if (i + 1 < argc) {
                process = new char[strlen(argv[i + 1]) + 1];
                strcpy(process, argv[i + 1]);
                i++;  // Skip next argument (process type)
            } else {
                std::cerr << "Error: -p flag requires a process name." << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "-fa") == 0 || strcmp(argv[i], "--flowaccumulation") == 0) {
            totalFlow = true;
        }
        else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--watershed") == 0) {
            watershed = true;
            if (i + 1 < argc) {
                if (isValidInteger(argv[i + 1])) {
                    nPourPoints = std::atoi(argv[i + 1]);
                    i++;
                    if (nPourPoints <= 0) {
                        std::cerr << "Error: -w flag requires a positive integer greater than 0 for Pour Points." << std::endl;
                        return false;
                    }
                }
                else {
                    std::cerr << "Error: -w flag requires a positive integer number of Pour Points." << std::endl;
                    return false;
                }
            }
            else {
                std::cerr << "Error: -w flag requires a positive integer number of Pour Points." << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_file = new char[strlen(argv[i + 1]) + 1];
                strcpy(output_file, argv[i + 1]);
                if (!(has_extension(output_file, "txt") || has_extension(output_file, "csv") || has_extension(output_file, "bin"))) {
                    std::cerr << "Error: Invalid output file extension. Supported extensions are .txt, .csv, .bin." << std::endl;
                    delete[] output_file;
                    return 1;
                }
                i++;  // Skip the next argument (output filename)
            } else {
                std::cerr << "Error: -o flag requires an output filename." << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "-img") == 0 || strcmp(argv[i], "--image") == 0) {
            if (i + 1 < argc) {
                image_file = new char[strlen(argv[i + 1]) + 1];
                strcpy(image_file, argv[i + 1]);
                if (!has_extension(image_file, "bmp")) {
                    std::cerr << "Error: Image file must have a .bmp extension." << std::endl;
                    delete[] image_file;
                    return false;
                }
                i++;  // Skip the next argument (image filename)
            } else {
                std::cerr << "Error: -img flag requires an image filename." << std::endl;
                return false;
            }
        }
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--colour") == 0) {
            if (i + 1 < argc && argv[i + 1][0] != '-') {  // Check if the next argument is not another flag
                colour_type = new char[strlen(argv[i + 1]) + 1];
                strcpy(colour_type, argv[i + 1]);
                colour = true;
                i++;  // Skip next argument (colour or flag)
            } else {
                std::cout << "No colour specified. Defaulting to greyscale (g1)." << std::endl;
                colour_type = new char[3];
                strcpy(colour_type, "g1");
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
        
        
        else {
            std::cerr << "Error: Unknown flag: " << argv[i] << std::endl;
            return false;
        }
    }

    return true;
}
