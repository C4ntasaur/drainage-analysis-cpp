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
    ```

2. Navigate into the project directory:

    ```bash
    cd drainage-analysis-cpp
    ```

3. Create a build directory and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

4. Run CMake to generate the build system:

    ```bash
    cmake ..
    ```

5. Build the project:

    ```bash
    make
    ````

## Usage

Instructions coming soon. Working on a combined CLI / REPL use.

### CLI Mode

To use the program in Command-Line Interface (CLI) mode:
    ```bash
    ./drainage-analysis
    ```
The program accepts several flags.

#### Flags

- **Help:**  Display help information.

    ```bash
    ./drainage_analysis -h
    ```

- **Input:** Input a DEM file

    ```bash
    ./drainage_analysis -i filename
    ```

    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with DEM width and height given as an integer header

- **Process:** Run a process on the DEM file

    ```bash
    ./drainage_analysis -p process
    ```

    Accepted processes:
    - Slope
    - Aspect
    - d8
    - dinf
    - mdf

- **Flow Accumulation:** Calculate flow accumulation based on Process.

    ```bash
    ./drainage_analysis -fa
    ```

    Only works with flow direction algorithms: d8, dinf, and mdf.
    Outputs images of log scaling or saved files of regular flow accumulation.
    Is incompatible with watershed delineation.

- **Watershed Delineation:** Calculate Watershed Delineation areas from a Process.

    ```bash
    ./drainage_analysis -w nPoints directory colour
    ```

    Only works with flow direction algorithms: d8, dinf, and mdf.
    Output is a series of images (number of points) of watersheds saved to specified directory with chosen colourmap.
    Images have log scaling applied.
    Is incompatible with other output methods (-o, -img).

- **File Saving:** Save the processed DEM.

    ```bash
    ./drainage_analysis -o filename
    ```

    Export processed DEM as a plain text (e.g. .txt, .csv) or binary file.
    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with Map width and height given as an integer header

- **Image Export:** Run in verbose mode to get detailed output.

    ```bash
    ./drainage_analysis -img filename
    ```

    Export processed DEM as a .bmp image.
    Can be used in conjunction with colourmaps with the -c flag.

- **Colour:** Run in verbose mode to get detailed output.

    ```bash
    ./drainage_analysis -c colourCode
    ```

    Select colour for image export (-img).
    If not specified, 'g1' (greyscale) is used.
    Colourmap short codes are found [here](data/colourmaps/).

    Example colourmaps:
    - 'g1', greyscale where white is the greatest value
    - 'sf', bluescale where dark blue is the greatest value
    - 'dw', brown - purple colourmap.

    Credit to Fabio Crameri for 'sf' (seafloor) and 'dw' (drywet) colourmaps.

#### Example Usage

##### Standard Flow Accumulation using multi-directional flow algorithm

    ```bash
        ./drainage-analysis -i ../data/DEMs/DTM50.txt -p mdf -fa -img ../outdir/flow.bmp -c g2
    ```

    This example shows how to create a flow accumulation image usinf the multi-directional flow algorithm.


### REPL / Interactive Mode

REPL mode is activate by the `-int` or `--interactive` flags as so:

    ```bash
    ./drainage_analysis -int
    ```

or

    ```bash
    ./drainage_analysis --interactive
    ```

Note: These flags are incompatible with all other flags.

## License

This project is licensed under the MIT License - see the [License](LICENSE) file for details.
