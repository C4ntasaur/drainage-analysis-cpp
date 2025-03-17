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
6. [Credits](#credits)
7. [License](#license)

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

| Flag | Description                | Arguments                             | Example                          |
|------|----------------------------|---------------------------------------|----------------------------------|
| `-i`  | Input DEM file            | `<filename>`                          | `-i data/DEMs/example.txt`       |
| `-p`  | Process to run            | `slope`, `aspect`, `d8`, `dinf`, `mdf`| `-p dinf`                        |
| `-fa` | Compute flow accumulation | None                                  | `-fa`                            |
| `-w`  | Watershed delineation     | `<num_points> <output_dir> <colourmap>`,  `[Colour Codes](#colourmaps)`                  | `-w 3 outputs/ sf`               |
| `-o`  | Save processed DEM        | `<filename>`                          | `-o output.csv`                  |
| `-img`| Export as BMP image       | `<filename>`                          | `-img flow.bmp`                  |
| `-c`  | Colourmaps for images     | [Colour Codes](#colourmaps)         | `-c dw`                          |
| `-h`  | Show help                 |  None                                 | `-h`                             |
| `-v`  | Enter verbose mode        | None                                  | `-v`                             |

Note:

- **D8 (`d8`) and D-Infinity (`dinf`):** By default these processes return output flow maps, Directional 8 and Aspect Maps respectively. Including the `-fa` flag computes flow accumulation instead.
- **Multi-Directional Flow (`mdf`)** does not output a flow map by default. Use `-fa` or `-w` to generate results.
#### Valid Processes:
- `slope`: Compute slope.
- `aspect`: Compute aspect.
- `d8`: Find the Directional 8 Map. Allows for flow accumulation (`-fa`) and watershed (`-w`).
- `dinf`: Finds the aspect map. Allows for flow accumulation (`-fa`) and watershed (`-w`).
- `mdf`: Allows for flow accumulation (`-fa`) and watershed (`-w`).

#### Colourmaps

Predefined colourmaps (stored in `data/colourmaps/`):
- `g1`: Greyscale (white = high)
- `g2`: Greyscale (black = high)
- `sf`: Bluescale ('seafloor')
- `dw`: Brown - purple ('drywet')

### REPL / Interactive Mode

Start an interactive session:

```bash
./drainage-analysis --interactive
```

#### Commands

| Command   | Description | Arguments                | Example
|-----------|----------------------------|------------|-------------------------|
| `load`    | Load a DEM file   | `<filename>`          | `load ../data/DEMs/DTM50.txt `      |
| `process` | Run a process. Check [Valid Processes](#valid-processes) | `[processes]`         | `process aspect`                    |
| `save`    | Save processed data | `<filename>`       | `save output.txt`                   |
| `export`  | Export as BMP            | `<filename>` | `export flow.bmp g1`                  |
| `help`    | Show commands      | None        | `help`                          |
| `exit`    | Quit REPL           | None      | `quit`                          |

#### Valid Processes:
- `slope`: Compute slope.
- `aspect`: Compute aspect.
- `d8`: Find the Directional 8 Map.
- `d8_flow`: Compute flow accumulation based on the Directional 8 method.
- `dinf_flow`: Compute flow accumulation based on the D-Infinity method.
- `mdf_flow`: Compute flow accumulation based on the Multi-Directional Flow method.
- `watershed`: Enter watershed delineation dialogue.

#### Watershed Workflow
```bash
> load data/DEMs/DTM50.txt
> process watershed
Enter process (d8/dinf/mdf): d8
Number of pour points: 3
Output directory: outputs/
Color map: sf
Watershed images saved to outputs/
```

### Examples

#### CLI Examples
1. **Flow Accumulation with MDF:**
    ```bash
    ./drainage-analysis -i data/DEMs/DTM50.txt -p mdf -fa -img flow.bmp -c g2
    ```
    Exports a greyscale BMP of flow accumulation.

    ![Flow Accumulation](docs/images/flow_accumulation_mdf.bmp)

3. **Watershed Delineation with D8:**
    ```bash
    ./drainage-analysis -i data/DEMs/DTM50.txt -p d8 -w 3 outputs/ sf
    ```
    Generates 3 watershed images in `outputs/` using the "seafloor" color map.

#### REPL Examples
1. **Flow Accumulation with MDF:**
    ```bash
    ./drainage-analysis --interactive
    Welcome to the DEM Processor REPL. Type 'help' for a list of commands.
    > load data/DEMs/DTM50.txt
    File loaded successfully.
    > process mdf_flow	
    MDF Flow accumulation completed.
    > export flow.bmp g2
    Flow map exported to flow.bmp
    > quit
    Exiting...
    Successful
    ```
    Exports a greyscale BMP of flow accumulation.

3. **Watershed Delineation with D8:**
    ```bash
    ./drainage-analysis --interactive
    Welcome to the DEM Processor REPL. Type 'help' for a list of commands.
    > load data/DEMs/DTM50.txt
    File loaded successfully.
    > process watershed
    Entering watershed mode
    Enter name of process to be used: d8
    Enter number of pour points: 3
    Enter directory to store watershed images: ../test/
    Using directory: ../outputs/
    Enter colourmap for watershed images: sf
    Using colourmap: sf
    Exported watershed images to: ../test/
    > quit
    Exiting...
    Successful
    ```

    Generates 3 watershed images in `outputs/` using the "seafloor" color map.

   <div style="display: flex; justify-content: space-between; gap: 10px">
      <img src="docs/images/watershed_0.bmp" alt="Watershed Image 1" style="width: 32%">
      <img src="docs/images/watershed_1.bmp" alt="Watershed Image 2" style="width: 32%">
      <img src="docs/images/watershed_2.bmp" alt="Watershed Image 3" style="width: 32%">
    </div>

## Credits
- Fabio Crameri for the sf (seafloor) and dw (drywet) color maps.
    Learn more about scientific color maps [here](https://www.fabiocrameri.ch/colourmaps/).
- Tarboton, 1997 for the D Infinity flow algorithm. <i>Tarboton, David G. "A new method for the determination of flow directions and upslope areas in grid digital elevation models." Water resources research 33.2 (1997): 309-319.</i>

## License

This project is licensed under the MIT License - see the [License](LICENSE) file for details.
