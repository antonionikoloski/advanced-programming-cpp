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

/*
 * This file implements the Image class, which represents a 2D image with support for loading,
 * saving, and manipulating pixel data. We created this class as part of our project to handle
 * image processing tasks, such as applying filters (e.g., grayscale, blur, edge detection) to
 * images. The class uses the stb_image library for loading and saving images in various formats
 * (PNG, JPG, BMP, TGA) and provides methods to access and modify pixel data.
 *
 * The Image class manages the image's width, height, number of channels, and pixel data, ensuring
 * proper memory management through its constructor, destructor, and move semantics. It also includes
 * a verbose mode to provide feedback about operations like loading and saving.
 */
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


Image::Image()
    : width(0), height(0), channels(0), data(nullptr), verbose(true)
{
}

Image::Image(int width, int height, int channels, unsigned char* data)
    : width(width), height(height), channels(channels), data(data), verbose(true)
{
}
Image::~Image() {
    clear();
}

Image::Image(Image&& other) noexcept
    : width(other.width),
      height(other.height),
      channels(other.channels),
      data(other.data),
      verbose(other.verbose)
{
    // Reset the other object's members to a safe state.
    other.width    = 0;
    other.height   = 0;
    other.channels = 0;
    other.data     = nullptr;
    other.verbose  = true;
}

Image& Image::operator=(Image&& other) noexcept {
    // Check for self-assignment to avoid unnecessary work.
    if (this != &other) {
        // Free any existing data in this object.
        clear();

        // Transfer ownership of the data and metadata.
        width    = other.width;
        height   = other.height;
        channels = other.channels;
        data     = other.data;
        verbose  = other.verbose;

        // Reset the other object's members to a safe state.
        other.width    = 0;
        other.height   = 0;
        other.channels = 0;
        other.data     = nullptr;
        other.verbose  = true;
    }
    return *this;
}

// ----------------------------------------------------------------------------
// Load
// ----------------------------------------------------------------------------

/*
 * Loads an image from a file into this Image object.
 * This method uses the stb_image library to load the image data and supports various formats
 * (e.g., PNG, JPG). It also allows specifying the desired number of channels for the loaded image.
 * Parameters:
 *   filename: The path to the image file to load.
 *   desiredChannels: The desired number of channels (0 to use the image's native channels, or 1-4 to force a specific number).
 * Returns: True if the image was loaded successfully, false otherwise.
 */
bool Image::load(const std::string& filename, int desiredChannels) {
    // Free any previously loaded data to prevent memory leaks.
    clear();

    // Use stb_image to load the image data from the file.
    data = stbi_load(filename.c_str(), &width, &height, &channels, desiredChannels);
    if (!data) {
        // If loading fails, print an error message and return false.
        std::cerr << "[Error] Failed to load image: " << filename << std::endl;
        return false;
    }

    // If a specific number of channels was requested, update the channels member accordingly.
    if (desiredChannels > 0) {
        channels = desiredChannels;
    }

    // If verbose mode is enabled, print information about the loaded image.
    if (verbose) {
        std::cout << "[Info] Loaded image: " << width << " x " << height
                  << " with " << channels << " channel(s) from: "
                  << filename << std::endl;
    }
    return true;
}

// ----------------------------------------------------------------------------
// Save
// ----------------------------------------------------------------------------

/*
 * Saves the image to a file.
 * This method uses the stb_image_write library to save the image in various formats (PNG, JPG, BMP, TGA),
 * determined by the file extension. It checks for valid data and supported extensions before saving.
 * Parameters:
 *   filename: The path to save the image to.
 * Returns: True if the image was saved successfully, false otherwise.
 */
bool Image::save(const std::string& filename) {
    // Check if there is any image data to save.
    if (!data) {
        std::cerr << "[Error] No image data to save.\n";
        return false;
    }

    // Extract the file extension from the filename.
    std::string ext;
    std::size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        // If no extension is found, print an error and return false.
        std::cerr << "[Error] Filename does not have an extension: " << filename << "\n";
        return false;
    }
    ext = filename.substr(pos + 1);
    // Convert the extension to lowercase for case-insensitive comparison.
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    int success = 0;
    // Save the image based on the file extension.
    if (ext == "png") {
        // Save as PNG with the specified dimensions and stride (width * channels).
        success = stbi_write_png(filename.c_str(), width, height, channels, data, width * channels);
    } else if (ext == "jpg" || ext == "jpeg") {
        // Save as JPG with a quality of 90 (out of 100).
        success = stbi_write_jpg(filename.c_str(), width, height, channels, data, 90);
    } else if (ext == "bmp") {
        // Save as BMP.
        success = stbi_write_bmp(filename.c_str(), width, height, channels, data);
    } else if (ext == "tga") {
        // Save as TGA.
        success = stbi_write_tga(filename.c_str(), width, height, channels, data);
    } else {
        // If the extension is not supported, print an error and return false.
        std::cerr << "[Error] Unsupported file extension: " << ext << std::endl;
        return false;
    }

    // Check if the save operation was successful.
    if (!success) {
        std::cerr << "[Error] Failed to save image: " << filename << std::endl;
        return false;
    }

    // If verbose mode is enabled, print a confirmation message.
    if (verbose) {
        std::cout << "[Info] Image saved as " << filename << std::endl;
    }
    return true;
}

/*
 * Retrieves a pointer to the pixel data at the specified coordinates.
 * This method allows modifying the pixel data directly and performs bounds checking to ensure
 * the coordinates are valid.
 * Parameters:
 *   x: The x-coordinate of the pixel.
 *   y: The y-coordinate of the pixel.
 * Returns: A pointer to the pixel data, or nullptr if the coordinates are out of bounds.
 */
unsigned char* Image::getPixel(int x, int y) {
    // Check if the coordinates are within the image bounds.
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return nullptr;
    }
    // Calculate the memory address of the pixel based on its coordinates and the number of channels.
    return data + (y * width + x) * channels;
}

/*
 * Retrieves a const pointer to the pixel data at the specified coordinates.
 * This method allows read-only access to the pixel data and performs bounds checking.
 * Parameters:
 *   x: The x-coordinate of the pixel.
 *   y: The y-coordinate of the pixel.
 * Returns: A const pointer to the pixel data, or nullptr if the coordinates are out of bounds.
 */
const unsigned char* Image::getPixel(int x, int y) const {
    // Check if the coordinates are within the image bounds.
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return nullptr;
    }
    // Calculate the memory address of the pixel based on its coordinates and the number of channels.
    return data + (y * width + x) * channels;
}

void Image::setPixel(int x, int y, const unsigned char* pixel) {
    // Get the memory address of the pixel.
    unsigned char* dest = getPixel(x, y);
    if (dest) {
        // Copy the pixel data into the image.
        std::memcpy(dest, pixel, static_cast<size_t>(channels));
    }
}

/*
 * Clears the image data and resets its metadata.
 * This method frees the allocated memory for the image data (if any) and resets the width, height,
 * and channels to 0, ensuring the object is in a clean state.
 */
void Image::clear() {
    // Check if there is any data to free.
    if (data) {
        // Use stb_image_free to free the memory, as it was allocated by stbi_load.
        stbi_image_free(data);
        data = nullptr;
    }
    // Reset the metadata.
    width    = 0;
    height   = 0;
    channels = 0;
}

void Image::setData(const unsigned char* newData, int newWidth, int newHeight, int newChannels) {
    // Clear existing data to prevent memory leaks.
    clear();
 
    // Check if the provided data is valid.
    if (!newData) {
        std::cerr << "[Error] Invalid data provided to setData().\n";
        return;
    }
 
    // Calculate the total size of the new data.
    const size_t dataSize = static_cast<size_t>(newWidth) * newHeight * newChannels;
    // Allocate new memory for the image data.
    data = static_cast<unsigned char*>(std::malloc(dataSize));
 
    // Check if memory allocation was successful.
    if (!data) {
        std::cerr << "[Error] Failed to allocate memory for new image data.\n";
        return;
    }
 
    // Copy the new data into the allocated memory.
    std::memcpy(data, newData, dataSize);
 
    // Update the image's metadata.
    width = newWidth;
    height = newHeight;
    channels = newChannels;
 
    // If verbose mode is enabled, print information about the updated image.
    if (verbose) {
        std::cout << "[Info] Image data updated: " << width << " x " << height
                  << " with " << channels << " channel(s)." << std::endl;
    }
}