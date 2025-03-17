# DrainageAnalysisCPP

A C++ implementation for drainage analysis, capable of processing Digital Elevation Models (DEMs) to compute terrain features, flow accumulation, and watershed delineation using multiple flow algorithms.
Created for Applied Programming module in ESE, Imperial College London.

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Installation](#installation)
4. [Usage](#usage)
    - [CLI Mode](#cli-mode)
    - [REPL Mode](#repl--interactive-mode)
5. [Examples](#examples)
6. [Contributing](#contributing)
7. [Credits](#credits)
8. [License](#license)

## Overview

**DrainageAnalysisCPP** is a tool for hydrological terrain analysis. It processes DEMs to compute slope, aspect, flow direction (using D8, D-Infinity, and Multi-Directional Flow algorithms), flow accumulation, and watershed delineation. Results can be exported as images, text files, or binary data.

## Features

- **Flow Direction Algorithms**
    - Directional 8 (D8)
    - D-Infinity (Dinf)
    - Multi-Directional Flow (MDF)
- **Terrain Analysis**
    - Slope Calculation (Sobel Gradient)
    - Aspect Calculation
- **Hydrological Tools**
    - Flow Accumulation
    - Watershed Delineation
- **Input / Output Formats**
    - Text (.txt), CSV (.csv), and binary (.bin) DEMs
    - BMP image exports with customizable colourmaps
- **Modes**
    - Command-Line Interface (CLI)
    - Interactive REPL (Read-Eval-Print Loop)

## Installation

### Prerequisites

- [CMake](https://cmake.org/install/) (version 3.31.6 or higher)
- A C++ compiler (GCC, Clang, or MSVC)

### Steps to Install

1. Clone this repository:

    ```bash
    git clone https://github.com/C4ntasaur/drainage-analysis-cpp
    cd drainage-analysis-cpp
    ```

2. Build the project:

    ```bash
    mkdir build && cd build
    cmake .. && make
    ```

Exectuable `drainage-analysis` will be in the `build` directory.

## Usage

### CLI Mode

Run the program with flags to specify inputs, processes, and outputs.
**General Syntax:**
```bash
./drainage-analysis [FLAGS] [ARGUMENTS]
```

#### Flags & Arguments

| Flag | Description                | Arguments                  | Example                          |
|------|----------------------------|----------------------------|----------------------------------|
| `-i` | Input DEM file             | `filename`                 | `-i data/DEMs/example.txt`       |
| `-p` | Process algorithm          | `d8`/`dinf`/`mdf`          | `-p dinf`                        |
| `-w` | Watershed delineation      | `nPoints dir colour`       | `-w 3 outputs/ sf`               |









To use the program in Command-Line Interface (CLI) mode:

The method accepts several flags.

#### Flags

- **Help:**  Display help information.

    ```bash
    ./drainage-analysis -h
    ```

- **Input:** Input a DEM file

    ```bash
    ./drainage-analysis -i filename
    ```

    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with DEM width and height given as an integer header

- **Process:** Run a process on the DEM file

    ```bash
    ./drainage-analysis -p process
    ```

    Accepted processes:
    - Slope, Sobel gradient of DEM.
    - Aspect, Aspect map of DEM.
    - d8, Directional 8 map of DEM.
    - dinf, D/infinity map of DEM
    - mdf, requires flow accumulation or watershed.

- **Flow Accumulation:** Calculate flow accumulation based on Process.

    ```bash
    ./drainage-analysis -fa
    ```

    Only works with flow direction algorithms: d8, dinf, and mdf.
    Outputs images of log scaling or saved files of regular flow accumulation.
    Is incompatible with watershed delineation.

- **Watershed Delineation:** Calculate Watershed Delineation areas from a Process.

    ```bash
    ./drainage-analysis -w nPoints directory colour
    ```

    Only works with flow direction algorithms: d8, dinf, and mdf.
    Output is a series of images (number of points) of watersheds saved to specified directory with chosen colourmap.
    Images have log scaling applied.
    Is incompatible with other output methods (-o, -img).

- **File Saving:** Save the processed DEM.

    ```bash
    ./drainage-analysis -o filename
    ```

    Export processed DEM as a plain text (e.g. .txt, .csv) or binary file.
    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with Map width and height given as an integer header

    This is incompatible with a bare multidirectional flow process (-p mdf).

- **Image Export:** Run in verbose mode to get detailed output.

    ```bash
    ./drainage_analysis -img filename
    ```

    Export processed DEM as a .bmp image.
    Can be used in conjunction with colourmaps with the -c flag.

- **Colour:** Run in verbose mode to get detailed output.

    ```bash
    ./drainage-analysis -c colourCode
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

This example shows how to create a flow accumulation image usinf the multi-directional flow algorithm.

```bash
./drainage-analysis -i ../data/DEMs/DTM50.txt -p mdf -fa -img ../outdir/flow.bmp -c g2
```

##### Watershed delineation of 3 points using Directional-8 algorithm

This example shows how to create a three watershed delineation images using the Directional-8 algorithm.
Here we do not use typical output flags such as -o or -img and only use -w.

```bash
./drainage-analysis -i ../data/DEMs/DTM50.txt -p d8 -w 3 ../outdir/ sf
```


### REPL / Interactive Mode

REPL mode is activate by the `-int` or `--interactive` flags as so:

```bash
./drainage-analysis -int
```

or

```bash
./drainage-analysis --interactive
```

Once ran you enter REPL mode:

```bash
Welcome to the DEM Processor REPL. Type 'help' for a list of commands.
> 
```

#### REPL Commands

There are several commands avaliable for REPL use.

- **load:** Load a DEM for processing

    ```bash
    > load filename
    ```

    Load a DEM from a plain text (e.g. .tx, .csv) or binary file.
    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with Map width and height given as an integer header

- **process:** Run specific processes on the loaded DEM

    ```bash
    > process process_name
    ```

- **watershed:** Sub-menu within the process command
    Accessed with `> process watershed` after loading a DEM.
    Will ask a series of questions on watershed parameters:

    ```bash
    Entering watershed mode
    Enter name of process to be used:
    Enter number of pour points:
    Enter directory to store watershed images:
    Using directory:
    Enter colourmap for watershed images:
    Using colourmap:
    Exported watershed images to:
    ```

- **save:** Save a processed DEM to a file.

    ```bash
    > save filename
    ```

    Save a processed map to a plain text (e.g. .tx, .csv) or binary file.
    Accepted file types:
    - .txt, space separated values
    - .csv, comma separated values
    - .bin, binary file with Map width and height given as an integer header

- **export:** Export a processed DEM as an image

    ```bash
    > export filename.bmp colour
    ```

    Export a processed map to a .bmp image.
    Colour parameter refers to the colour short codes found [here](data/colourmaps/).

- **help:** Prints a help message for command usage

    ```bash
    > help
    ```

## License

This project is licensed under the MIT License - see the [License](LICENSE) file for details.
