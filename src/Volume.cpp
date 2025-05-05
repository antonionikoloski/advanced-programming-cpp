/*
 * Group Name: Dijkstra
 * Members:
 *   - Antonio Nikoloski (GitHub: esemsc-an1224)
 *   - Tyana Tshiota (GitHub: esemsc-tnt24)
 *   - Yue Zigi (GitHub: esemsc-zy424)
 *   - Yang Aijia (GitHub: esemsc-ay424)
 *   - Zhou Xiaorui (GitHub: esemsc-zx24)
 *   - Zhang Zewei (GitHub: esemsc-zz724)
 *   - Azam Sazina (GitHub: esemsc-as1224)
 */
#include "Volume.h"
#include "stb_image.h"
#include "Image.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

// Stores parsed filename pattern information
struct FilenamePattern {
    std::string prefix; // File name prefix (basename)
    int numDigits;      // Number of digits in the numeric part
    int startNumber;    // Starting value of the numeric part
};

// Parses the prefix, number of digits, and starting number from the filename
FilenamePattern parseFilename(const std::string& filename, const std::string& basename) {
    // Remove file extension
    size_t dotPos = filename.rfind('.');
    std::string base = (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);

    // Check if the filename starts with the basename
    if (base.size() < basename.size() || base.substr(0, basename.size()) != basename) {
        return { "", 0, 0 }; // Not a match
    }

    // Extract the numeric part after the basename
    std::string digitPart = base.substr(basename.size());
    if (digitPart.empty() || !std::all_of(digitPart.begin(), digitPart.end(), ::isdigit)) {
        return { "", 0, 0 }; // No numeric part
    }

    int startNumber = std::atoi(digitPart.c_str());
    int numDigits = static_cast<int>(digitPart.size());

    return { basename, numDigits, startNumber };
}

Volume::Volume()
    : width(0), height(0), depth(0), channels(0) {
}

Volume::~Volume() {
    // Each Image's destructor automatically frees its data.
}

bool Volume::load(const std::string& directoryWithBasename) {
    slices.clear();
    width = 0;
    height = 0;
    depth = 0;
    channels = 0;

    // Extract directory and basename from the input path
    fs::path path(directoryWithBasename);
    std::string directory = path.parent_path().string();
    std::string basename = path.filename().string();

    if (directory.empty()) {
        directory = ".";
    }

    // Check if the directory exists
    fs::path dirPath(directory);
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Directory does not exist: " << directory << std::endl;
        return false;
    }

    // Define supported file extensions
    const std::vector<std::string> validExtensions = { ".png", ".jpg", ".jpeg" };

    // Collect all files that match the basename + number + extension pattern
    std::vector<std::pair<int, std::string>> files; // Pair of (number, filename)
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (std::find(validExtensions.begin(), validExtensions.end(), ext) != validExtensions.end()) {
                FilenamePattern pattern = parseFilename(filename, basename);
                if (pattern.numDigits > 0) {
                    files.emplace_back(pattern.startNumber, filename);
                }
            }
        }
    }

    if (files.empty()) {
        std::cerr << "No image files matching basename '" << basename << "' found in directory: " << directory << std::endl;
        return false;
    }

    // Sort files by their numeric part
    std::sort(files.begin(), files.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    // Debug: Print the number of files found
    std::cout << "Found " << files.size() << " image files in directory: " << directory << "\n";
    for (const auto& file : files) {
        std::cout << "File: " << file.second << " (number: " << file.first << ")\n";
    }

    // Load all slices
    int loadedCount = 0;
    for (const auto& file : files) {
        std::string filename = directory + "/" + file.second;
        Image slice;
        if (!slice.load(filename, 0)) {
            std::cerr << "Failed to load slice: " << filename << std::endl;
            continue; // Skip this slice, but keep loading others
        }

        if (loadedCount == 0) {
            width = slice.getWidth();
            height = slice.getHeight();
            channels = slice.getChannels();
        } else {
            if (slice.getWidth() != width ||
                slice.getHeight() != height ||
                slice.getChannels() != channels) {
                std::cerr << "Slice file " << filename
                          << " does not match volume dimensions/channels. Skipping.\n";
                continue;
            }
        }

        slices.push_back(std::move(slice));
        loadedCount++;
    }

    depth = loadedCount;
    if (depth == 0) {
        std::cerr << "Failed to load any slices from directory: " << directory << std::endl;
        return false;
    }

    std::cout << "Loaded volume from directory '" << directory
              << "' with " << width << " x " << height
              << " x " << depth << ", channels = " << channels << std::endl;
    return true;
}

int Volume::getWidth() const {
    return width;
}

int Volume::getHeight() const {
    return height;
}

int Volume::getDepth() const {
    return depth;
}

int Volume::getChannels() const {
    return channels;
}

std::vector<Image>& Volume::getSlices() {
    return slices;
}

const std::vector<Image>& Volume::getSlices() const {
    return slices;
}

unsigned char* Volume::getVoxel(int x, int y, int z) {
    if (z < 0 || z >= depth)   return nullptr;
    if (y < 0 || y >= height)  return nullptr;
    if (x < 0 || x >= width)   return nullptr;
    return slices[z].getPixel(x, y);
}

void Volume::setVoxel(int x, int y, int z, const unsigned char* pixel) {
    if (z < 0 || z >= depth)   return;
    if (y < 0 || y >= height)  return;
    if (x < 0 || x >= width)   return;
    slices[z].setPixel(x, y, pixel);
}