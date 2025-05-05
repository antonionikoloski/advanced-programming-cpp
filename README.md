# APImageFilters
Advanced Programming Image Filters Project

[![License](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)  
[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/w/)  
[![Status](https://img.shields.io/badge/Status-Active%20Development-orange)]()

## CT Scans
Download CT Scan datasets here:

https://imperiallondon-my.sharepoint.com/:u:/g/personal/tmd02_ic_ac_uk/EafXMuNsbcNGnRpa8K62FjkBvIKvCswl1riz7hPDHpHdSQ

## Group Information
- **Group Name**: Dijkstra
- **Members**:
  - Antonio Nikoloski (GitHub: esemsc-an1224)
  - Azam Sazina (GitHub: esemsc-sa3224)
  - Tyana Tshiota (GitHub: esemsc-tnt24)
  - Yang Aijia (GitHub: esemsc-ay424)
  - Yue Zigi (GitHub: esemsc-zy424)
  - Zhang Zewei (GitHub: esemsc-zz724)
  - Zhou Xiaorui (GitHub: esemsc-zx24)

## Project Overview
`APImageFilters` is a C++ project developed as part of an Advanced Programming course. The project focuses on image processing and 3D volume visualization, providing tools to apply filters to 2D images and perform projections on 3D volumetric data, such as CT scans. Key features include:

- **Image Processing**: Load and save images in various formats (PNG, JPG, BMP, TGA) using the stb_image library, and apply filters like grayscale, blur, or edge detection.
- **3D Volume Projections**: Perform Maximum Intensity Projection (MIP), Minimum Intensity Projection (MinIP), Mean Intensity Projection (MeanIP), and Median Intensity Projection (MedianIP) on 3D volumes to create 2D visualizations.
- **Command-Line Interface**: The program is designed to be run from the command line with various options to specify input files, output files, and operations.
- **Unit Tests**: Includes a test suite to verify the correctness of image processing and projection functions.

## To Access Doxygen HTML

run:
```bash
open docs/html/index.html
```

## To Run Custom Unit Tests

run:
```bash
cd build
./runUnitTests
```

## Installation instructions

### MacOS or Linux

First, clone the repository:

```bash
git clone XXX
cd APImageFilters
```

Install cmake and a C++ compiler if you don't already have them. On MacOS, you can install them with Homebrew:

```bash
brew install cmake
brew install gcc
```

Then, create a build directory and run cmake:

```bash
cmake -S . -B build
```

Finally, compile the project:

```bash
cmake --build build
```

### Windows with MSVC

If you don't already have them installed on Windows (not WSL), you will need to install CMake and git. You can download CMake from [here](https://cmake.org/download/) and git from [here](https://git-scm.com/download/win).

First, in Windows Powershell, clone the repository:

```pwsh
git clone XXX
cd APImageFilters
```

Then, create a build directory and run cmake:

```pwsh
cmake -S . -B build -G "Visual Studio 17 2022"
```

Replace "Visual Studio 17 2022" with the appropriate version if you are using a different version of Visual Studio.

Open the generated solution file (.sln) in Visual Studio:

```pwsh
start build\APImageFilters.sln
```
