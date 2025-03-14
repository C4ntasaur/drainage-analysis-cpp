# DrainageAnalysisCPP

Drainage analysis written in C++ for the Advanced Programming coursework section.

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Usage](#usage)
4. [License](#license)

## Overview

This project is a C++ implementation of a drainage analysis tool used to calculate terrain features, flow accumulation, and watershed areas of Digital Elevation Models (DEMs) across many flow algorithms.

## Installation

### Prerequisites

- [CMake](https://cmake.org/install/) (version 3.31.6 or higher)
- A C++ compiler (e.g., GCC, Clang, or MSVC)

### Steps to Install

1. Clone this repository:

    ```bash
    git clone https://github.com/C4ntasaur/drainage-analysis-cpp

2. Navigate into the project directory:

    ```bash
    cd drainage-analysis-cpp

3. Create a build directory and navigate into it:

    ```bash
    mkdir build
    cd build

4. Run CMake to generate the build system:

    ```bash
    cmake ..
    ```

5. Build the project:

    ```bash
    make

## Usage

Instructions coming soon. Working on a combined CLI / REPL use.

### CLI Mode

    
    ./drainage_analysis

Command Line Input of  drainage-analysis uses many flags

#### Help / `-h`

### REPL / Interactive Mode

REPL mode is activate by the `-int` or `--interactive` flags as so:

    
    ./drainage_analysis -int

    or

    
    ./drainage_analysis --interactive

These flags are incompatible with all other 

## License

This project is licensed under the MIT License - see the [License](LICENSE) file for details.
