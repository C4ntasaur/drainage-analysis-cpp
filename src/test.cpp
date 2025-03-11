#include "Map.h"
#include "BMP.h"
#include "SobelAnalysis.h"
#include "D8FlowAnalyser.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "No flags provided. Use -h for help." << std::endl;
        return 1;
    }
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << "Help: Use the following flags:\n";
            std::cout << "-h, --help : Show this help message\n";
            std::cout << "-v, --verbose : Enable verbose output\n";
            std::cout << "-f <filename> : Specify a file\n";
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            std::cout << "Verbose mode enabled.\n";
        }
        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
            if (i + 1 < argc) {
                std::cout << "File specified: " << argv[i + 1] << "\n";
                i++;
            } else {
                std::cerr << "Error: -f flag requires a filename argument.\n";
                return 1;
            }
        }
        else {
            std::cerr << "Unknown flag: " << argv[i] << "\n";
            return 1;
        }
    }




    /*
    Map<float> elevationData;
    elevationData.loadFromFile("../data/DTM50.txt", "txt");
    
    D8FlowAnalyser<float> analyser(elevationData);
    analyser.analyseFlow();
    Map <int> D8Map = analyser.getMap();
    //slopeMap.exportToImage("../data/test.bmp", "g2");
    D8Map.exportToImage("../data/D8.bmp", "g2");
    D8Map.saveToFile("../data/savetest1.txt", "txt");
    */
    return 0;
}
