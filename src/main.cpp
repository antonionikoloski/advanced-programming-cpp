// main.cpp
#include "stb_image.h"  // Include the header for declarations, but no implementation
#include "stb_image_write.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

#include "Image.h"
#include "Filter.h"        // your factory functions: createGreyscaleFilter(), etc.
#include "Volume.h"
#include "Projection.h"
#include "Slice.h"

// -------------------------------------------------------------------
// Structures to hold options for 2D and 3D modes
// -------------------------------------------------------------------
struct ProgramOptions2D {
    std::string inputFile;
    std::string outputFile;

    bool greyscale        = false;
    bool sharpen          = false;

    bool brightnessFlag   = false;
    bool autoBrightness   = false;  // true => no numeric offset => auto
    int brightnessValue   = 0;

    bool histogramFlag    = false;
    std::string histogramType;  // e.g., "HSV","HSL"

    bool thresholdFlag    = false;
    int thresholdValue    = 128;
    std::string thresholdMode;

    bool saltPepperFlag   = false;
    float saltPepperPercent = 0.0f;

    bool blurFlag         = false;
    std::string blurType;
    int blurKernelSize    = 3;
    double blurStdev      = 2.0;

    bool edgeFlag         = false;
    std::string edgeType;
};

struct ProgramOptions3D {
    std::string inputDir;
    std::string outputFile;

    bool blur3DFlag        = false;
    std::string blur3DType;
    int blur3DKernelSize   = 3;
    double blur3DStdev     = 2.0;

    bool projectionFlag    = false;
    std::string projectionType;

    bool sliceFlag         = false;
    std::string slicePlane;
    int sliceConstant      = 0;

    // For partial slab:
    bool slabRangeFlag     = false; // if user provided --zrange
    int slabZMin           = 0;     // 1-based
    int slabZMax           = 0;     // 1-based
};

// -------------------------------------------------------------------
// Print usage info
// -------------------------------------------------------------------
void printUsage(const std::string& progName) {
    std::cerr << "Usage:\n";
    std::cerr << "  2D mode: " << progName
              << " -i <input_image> [2D options] <output_image>\n\n";
    std::cerr << "    2D Options:\n"
              << "      --greyscale | -g\n"
              << "      --brightness [val] | -b [val]        (if val is omitted, use auto mode)\n"
              << "      --histogram <type> | -h <type>       (e.g., HSV, HSL)\n"
              << "      --blur <type> <size> [<stdev>] | -r <type> <size> [<stdev>]\n"
              << "               (type can be Box, Gaussian, or Median)\n"
              << "      --edge <type> | -e <type>            (Sobel, Prewitt, Scharr, RobertsCross)\n"
              << "      --sharpen | -p\n"
              << "      --saltpepper <percent> | -n <pct>\n"
              << "      --threshold <val> <mode> | -t <val> <mode>\n"
              << "         (e.g. 128 HSV)\n\n";

    std::cerr << "  3D mode: " << progName
              << " -d <input_volume_directory> [3D options] <output_image>\n\n";
    std::cerr << "    3D Options:\n"
              << "      --blur3d <type> <size> [<stdev>]    (type: Gaussian, Median)\n"
              << "      --projection <type> | -p <type>     (MIP, MinIP, MeanAIP, MedianAIP)\n"
              << "      --slice <plane> <constant> | -s <plane> <const>\n"
              << "         (plane: XZ or YZ, constant: coordinate in that plane)\n"
              << "      --zrange <start> <end>  (1-based z-slab for partial projection)\n\n";
}
void normalizeMinMax(Image& img) {
   std:: cout<<"Normalizing image to [0, 255] based on min/max values"<<std::endl;
    int w = img.getWidth();
    int h = img.getHeight();
    int ch = img.getChannels();
    unsigned char* data = img.getData();
    if (!data) return;

    // Find min and max
    int total = w * h * ch;
    int minVal = 255, maxVal = 0;
    for (int i = 0; i < total; i++) {
        int val = data[i];
        if (val < minVal) minVal = val;
        if (val > maxVal) maxVal = val;
    }

    int range = maxVal - minVal;
    if (range < 1) return; // everything’s the same or empty

    // Scale each pixel so minVal -> 0, maxVal -> 255
    for (int i = 0; i < total; i++) {
        int val = data[i] - minVal;
        val = static_cast<int>((val * 255.0) / range + 0.5);
        data[i] = static_cast<unsigned char>(std::clamp(val, 0, 255));
    }
}


// -------------------------------------------------------------------
// 2D Command line
// -------------------------------------------------------------------
bool parseCommandLine2D(int argc, char* argv[], ProgramOptions2D &opts) {
    if (argc < 4) {
        printUsage(argv[0]);
        return false;
    }

    // e.g.  ./prog -i input.jpg --greyscale --brightness 50 out.jpg
    // argv[1] = "-i", argv[2] = "input.jpg", 
    // then parse 2D options, last is "out.jpg"
    opts.inputFile = argv[2];
    int idx = 3;
    int last = argc - 1;
    opts.outputFile = argv[last];

    while (idx < last) {
        std::string opt = argv[idx++];
        if (opt == "--greyscale" || opt == "-g") {
            opts.greyscale = true;
        }
        else if (opt == "--brightness" || opt == "-b") {
            opts.brightnessFlag = true;
            if (idx < last) {
                // Peek next token
                std::string nextArg = argv[idx];
                try {
                    int val = std::stoi(nextArg);
                    opts.brightnessValue = val;
                    opts.autoBrightness = false;
                    idx++;
                }
                catch (...) {
                    // no numeric => auto
                    opts.autoBrightness = true;
                }
            } else {
                opts.autoBrightness = true;
            }
        }
        else if (opt == "--histogram" || opt == "-h") {
            if (idx < last) {
                opts.histogramFlag = true;
                opts.histogramType = argv[idx++];
            } else {
                std::cerr << "[Error] Missing histogram type after " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--blur" || opt == "-r") {
            if (idx + 1 < last) {
                opts.blurFlag     = true;
                opts.blurType     = argv[idx++];
                opts.blurKernelSize = std::stoi(argv[idx++]);
                if (opts.blurType == "Gaussian" && idx < last) {
                    // optional stdev
                    try {
                        double possibleStdev = std::stod(argv[idx]);
                        opts.blurStdev = possibleStdev;
                        idx++;
                    } catch (...) {
                        // ignore, use default
                    }
                }
            } else {
                std::cerr << "[Error] Not enough parameters after " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--edge" || opt == "-e") {
            if (idx < last) {
                opts.edgeFlag = true;
                opts.edgeType = argv[idx++];
            } else {
                std::cerr << "[Error] Missing edge detection type after " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--sharpen" || opt == "-p") {
            opts.sharpen = true;
        }
        else if (opt == "--saltpepper" || opt == "-n") {
            if (idx < last) {
                opts.saltPepperFlag = true;
                opts.saltPepperPercent = std::stof(argv[idx++]);
            } else {
                std::cerr << "[Error] Missing salt/pepper percentage after " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--threshold" || opt == "-t") {
            if (idx + 1 < last) {
                opts.thresholdFlag = true;
                opts.thresholdValue = std::stoi(argv[idx++]);
                opts.thresholdMode  = argv[idx++];
            } else {
                std::cerr << "[Error] Missing threshold parameters after " << opt << "\n";
                return false;
            }
        }
        else {
            std::cerr << "[Warning] Unknown 2D option: " << opt << "\n";
        }
    }

    return true;
}

// -------------------------------------------------------------------
// 3D Command line
// -------------------------------------------------------------------
bool parseCommandLine3D(int argc, char* argv[], ProgramOptions3D &opts) {
    if (argc < 4) {
        printUsage(argv[0]);
        return false;
    }

    opts.inputDir = argv[2];
    int idx = 3;
    int last = argc - 1;
    opts.outputFile = argv[last];

    while (idx < last) {
        std::string opt = argv[idx++];
        if (opt == "--blur3d") {
            if (idx + 1 < last) {
                opts.blur3DFlag     = true;
                opts.blur3DType     = argv[idx++];
                opts.blur3DKernelSize = std::stoi(argv[idx++]);
                if (opts.blur3DType == "Gaussian" && idx < last) {
                    try {
                        double possibleStdev = std::stod(argv[idx]);
                        opts.blur3DStdev = possibleStdev;
                        idx++;
                    } catch (...) {
                        // ignore
                    }
                }
            } else {
                std::cerr << "[Error] Not enough parameters for --blur3d\n";
                return false;
            }
        }
        else if (opt == "--projection" || opt == "-p") {
            if (idx < last) {
                opts.projectionFlag = true;
                opts.projectionType = argv[idx++];
            } else {
                std::cerr << "[Error] Missing projection type after " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--slice" || opt == "-s") {
            if (idx + 1 < last) {
                opts.sliceFlag = true;
                opts.slicePlane  = argv[idx++];
                opts.sliceConstant = std::stoi(argv[idx++]);
            } else {
                std::cerr << "[Error] Not enough parameters for " << opt << "\n";
                return false;
            }
        }
        else if (opt == "--zrange") {
            // user typed e.g. --zrange 10 20
            if (idx + 1 < last) {
                opts.slabRangeFlag = true;
                opts.slabZMin      = std::stoi(argv[idx++]); // user is 1-based
                opts.slabZMax      = std::stoi(argv[idx++]);
            } else {
                std::cerr << "[Error] Not enough parameters for --zrange\n";
                return false;
            }
        }
        else {
            std::cerr << "[Warning] Unknown 3D option: " << opt << "\n";
        }
    }

    return true;
}

// -------------------------------------------------------------------
// process2DImage
// -------------------------------------------------------------------
bool process2DImage(const ProgramOptions2D &opts) {
    // Load
    Image img;
    if (!img.load(opts.inputFile)) {
        std::cerr << "Failed to load image: " << opts.inputFile << std::endl;
        return false;
    }

    // Build a pipeline of Filter2D*
    std::vector<Filter2D*> filters2D;

    if (opts.greyscale) {
        filters2D.push_back(createGreyscaleFilter());
    }
    if (opts.brightnessFlag) {
        if (opts.autoBrightness) {
            filters2D.push_back(createBrightnessFilter()); // no-arg => auto
        } else {
            filters2D.push_back(createBrightnessFilter(opts.brightnessValue));
        }
    }
    if (opts.histogramFlag) {
        filters2D.push_back(createHistogramEqualisationFilter(opts.histogramType));
    }
    if (opts.blurFlag) {
        if (opts.blurType == "Box") {
            filters2D.push_back(createBoxBlurFilter(opts.blurKernelSize));
        }
        else if (opts.blurType == "Gaussian") {
            filters2D.push_back(createGaussianBlurFilter(opts.blurKernelSize, opts.blurStdev));
        }
        else if (opts.blurType == "Median") {
            filters2D.push_back(createMedianBlurFilter(opts.blurKernelSize));
        }
        else {
            std::cerr << "[Error] Unknown blur type: " << opts.blurType << "\n";
        }
    }
    if (opts.edgeFlag) {
        filters2D.push_back(createEdgeDetectionFilter(opts.edgeType));
    }
    if (opts.sharpen) {
        filters2D.push_back(createSharpenFilter());
    }
    if (opts.saltPepperFlag) {
        filters2D.push_back(createSaltPepperFilter(opts.saltPepperPercent));
    }
    if (opts.thresholdFlag) {
        filters2D.push_back(createThresholdFilter(opts.thresholdValue, opts.thresholdMode));
    }

    // Apply filters
    for (auto* f : filters2D) {
        f->apply(img);
    }

    // Save result
    if (!img.save(opts.outputFile)) {
        std::cerr << "Failed to save image: " << opts.outputFile << std::endl;
        for (auto* f : filters2D) delete f;
        return false;
    }

    // Cleanup
    for (auto* f : filters2D) {
        delete f;
    }
    filters2D.clear();

    return true;
}

// -------------------------------------------------------------------
// process3DVolume
// -------------------------------------------------------------------
bool process3DVolume(const ProgramOptions3D &opts) {
    // Load
    Volume vol;
    if (!vol.load(opts.inputDir)) {
        std::cerr << "Failed to load volume: " << opts.inputDir << std::endl;
        return false;
    }
    std::cout << "Loaded volume: "
              << vol.getWidth() << " x " << vol.getHeight()
              << " x " << vol.getDepth()
              << " with " << vol.getChannels() << " channel(s).\n";

    // Build pipeline of Filter3D*
    std::vector<Filter3D*> filters3D;
    if (opts.blur3DFlag) {
        if (opts.blur3DType == "Gaussian") {
            filters3D.push_back(createGaussianBlur3DFilter(opts.blur3DKernelSize, opts.blur3DStdev));
        }
        else if (opts.blur3DType == "Median") {
            filters3D.push_back(createMedianBlur3DFilter(opts.blur3DKernelSize));
        }
        else {
            std::cerr << "[Error] Unknown 3D blur type: " << opts.blur3DType << "\n";
            return false;
        }
    }

    // Apply 3D filters
    for (auto* f : filters3D) {
        f->apply(vol);
    }

    // Must do either projection or slice
    if (!opts.projectionFlag && !opts.sliceFlag) {
        std::cerr << "[Error] No volume processing option specified. Use --projection or --slice.\n";
        for (auto* f : filters3D) delete f;
        return false;
    }

    Image result;
    if (opts.projectionFlag) {
        // If user gave partial slab range => convert from 1-based to 0-based
        int zMin = 0;
        int zMax = vol.getDepth() - 1;
        if (opts.slabRangeFlag) {
            zMin = opts.slabZMin - 1;
            zMax = opts.slabZMax - 1;
            // clamp
            if (zMin < 0) zMin = 0;
            if (zMax >= vol.getDepth()) zMax = vol.getDepth()-1;
            if (zMin > zMax) {
                std::cerr << "[Error] slab z-range is invalid.\n";
                for (auto* f : filters3D) delete f;
                return false;
            }
        }

        if (opts.projectionType == "MIP") {
            result = Projection::maximumIntensityProjection(vol, zMin, zMax);
            normalizeMinMax(result);
        }
        else if (opts.projectionType == "MinIP") {
            result = Projection::minimumIntensityProjection(vol, zMin, zMax);
        }
        else if (opts.projectionType == "meanAIP") {
            result = Projection::meanIntensityProjection(vol, zMin, zMax);
        }
        else if (opts.projectionType == "medianAIP") {
            result = Projection::medianIntensityProjection(vol, zMin, zMax);
        }
        else {
            //- Projection: `--projection <type>` or `-p <type>` (e.g., MIP, MinIP, meanAIP, medianAIP)
            std::cerr << "[Error] Unknown projection type: " << opts.projectionType << "\n";
            for (auto* f : filters3D) delete f;
            return false;
        }
    }
    else if (opts.sliceFlag) {
        // Do a slice
        int planeCoord = opts.sliceConstant - 1;
        result = Slice::sliceVolume(vol, opts.slicePlane, planeCoord);
    }

    // Save
    normalizeMinMax(result);
    if (!result.save(opts.outputFile)) {
        std::cerr << "[Error] Failed to save processed volume image: " << opts.outputFile << "\n";
        for (auto* f : filters3D) delete f;
        return false;
    }
    std::cout << "Processed volume image saved as " << opts.outputFile << "\n";

    // cleanup
    for (auto* f : filters3D) delete f;
    return true;
}

// -------------------------------------------------------------------
// main
// -------------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string modeFlag = argv[1];
    if (modeFlag == "-i") {
        // 2D mode
        ProgramOptions2D options2D;
        if (!parseCommandLine2D(argc, argv, options2D)) {
            return 1;
        }
        if (!process2DImage(options2D)) {
            return 1;
        }
    }
    else if (modeFlag == "-d") {
        // 3D mode
        ProgramOptions3D options3D;
        if (!parseCommandLine3D(argc, argv, options3D)) {
            return 1;
        }
        if (!process3DVolume(options3D)) {
            return 1;
        }
    }
    else {
        std::cerr << "[Error] First parameter must be -i (2D image) or -d (3D volume)\n";
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
