#ifndef VOLUME_H
#define VOLUME_H

#include <string>
#include <vector>
#include "Image.h"

/**
 * @class Volume
 * @brief The Volume class is used to load and store 3D volume data (e.g., a series of sliced images).
 *
 * This class assumes the volume data is predominantly single-channel (greyscale) or RGB,
 * but multi-channel formats can also be supported. It provides methods for loading data,
 * accessing slices, and reading/writing voxel data directly.
 */
class Volume {
public:
    /**
     * @brief Default constructor that initializes an empty Volume object.
     */
    Volume();

    /**
     * @brief Destructor to release allocated resources.
     */
    ~Volume();

    /**
     * @brief Loads a 3D dataset from a specified directory.
     *
     * The implementation assumes that the slices are named sequentially (e.g., 001.png, 002.png, etc.).
     * The function continues loading until no more valid files are found.
     *
     * @param directory Directory path containing the image slices.
     * @return Returns true if at least one slice is loaded successfully, otherwise false.
     */
    bool load(const std::string& directory);

    /**
     * @brief Gets the width (in pixels) of each slice in the volume.
     * @return The width of the volume.
     */
    int getWidth() const;

    /**
     * @brief Gets the height (in pixels) of each slice in the volume.
     * @return The height of the volume.
     */
    int getHeight() const;

    /**
     * @brief Gets the number of slices (depth) in the volume.
     * @return The depth of the volume.
     */
    int getDepth() const;

    /**
     * @brief Gets the number of channels per pixel in the volume.
     * @return The number of channels.
     */
    int getChannels() const;

    /**
     * @brief Provides a reference to the internal slices for direct access.
     * @return A reference to the internal vector of Image slices.
     */
    std::vector<Image>& getSlices();

    /**
     * @brief Provides a constant reference to the internal slices for read-only access.
     * @return A constant reference to the internal vector of Image slices.
     */
    const std::vector<Image>& getSlices() const;

    /**
     * @brief Retrieves a pointer to a voxel at the specified (x, y, z) coordinates.
     *
     * This pointer can be used for direct reading and writing.
     * - `z` corresponds to the first slice (0-based indexing).
     * - `y` corresponds to the row.
     * - `x` corresponds to the column.
     *
     * @param x X-coordinate of the voxel.
     * @param y Y-coordinate of the voxel.
     * @param z Z-coordinate of the voxel (slice index).
     * @return Pointer to the voxel data or nullptr if out of bounds.
     */
    unsigned char* getVoxel(int x, int y, int z);

    /**
     * @brief Sets the voxel value at the specified (x, y, z) coordinates.
     *
     * Does nothing if the specified coordinates are out of bounds.
     *
     * @param x X-coordinate of the voxel.
     * @param y Y-coordinate of the voxel.
     * @param z Z-coordinate of the voxel.
     * @param pixel Pointer to the pixel data to set.
     */
    void setVoxel(int x, int y, int z, const unsigned char* pixel);

private:
    int width;                  ///< Width of each slice.
    int height;                 ///< Height of each slice.
    int depth;                  ///< Number of slices (depth of the volume).
    int channels;               ///< Number of channels per pixel.
    std::vector<Image> slices;  ///< Stores all slices as Image objects.
};

#endif // VOLUME_H