#ifndef IMAGE_H
#define IMAGE_H

#include <string>
// doxygen comments formatted with ChatGPT
/**
 * @class Image
 * @brief The Image class represents a 2D image with pixel data and basic manipulation features.
 *
 * This class supports loading, saving, and accessing image pixel data. It also provides
 * functionality for clearing data and managing verbosity for debugging purposes.
 */
class Image {
public:
    /**
     * @brief Default constructor. Initializes an empty image object.
     */
    Image();

    /**
     * @brief Parameterized constructor to create an image with specific dimensions and data.
     * @param width Image width in pixels.
     * @param height Image height in pixels.
     * @param channels Number of color channels (e.g., 3 for RGB, 4 for RGBA).
     * @param data Pointer to the raw image data.
     */
    Image(int width, int height, int channels, unsigned char* data);

    /**
     * @brief Destructor to release allocated resources.
     */
    ~Image();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    Image(const Image&) = delete;

    /**
     * @brief Deleted copy assignment operator to prevent copying.
     */
    Image& operator=(const Image&) = delete;

    /**
     * @brief Move constructor to transfer ownership of resources.
     * @param other Image object to move from.
     */
    Image(Image&& other) noexcept;

    /**
     * @brief Move assignment operator to transfer ownership of resources.
     * @param other Image object to move from.
     * @return Reference to the current object.
     */
    Image& operator=(Image&& other) noexcept;

    /**
     * @brief Loads an image from a file.
     * @param filename Path to the image file.
     * @param desiredChannels Number of channels to load (0 to keep original).
     * @return True if loading succeeds, false otherwise.
     */
    bool load(const std::string& filename, int desiredChannels = 0);

    /**
     * @brief Saves the current image to a file.
     * @param filename Path to save the image file.
     * @return True if saving succeeds, false otherwise.
     */
    bool save(const std::string& filename);

    /**
     * @brief Gets the width of the image in pixels.
     * @return Image width.
     */
    int getWidth() const { return width; }

    /**
     * @brief Gets the height of the image in pixels.
     * @return Image height.
     */
    int getHeight() const { return height; }

    /**
     * @brief Gets the number of channels in the image.
     * @return Number of channels.
     */
    int getChannels() const { return channels; }

    /**
     * @brief Gets the pointer to the image's raw data.
     * @return Pointer to mutable image data.
     */
    unsigned char* getData() { return data; }

    /**
     * @brief Gets a constant pointer to the image's raw data.
     * @return Constant pointer to image data.
     */
    const unsigned char* getData() const { return data; }

    /**
     * @brief Retrieves a pointer to the pixel at the specified (x, y) coordinates.
     * @param x X-coordinate of the pixel.
     * @param y Y-coordinate of the pixel.
     * @return Pointer to the pixel data or nullptr if out of bounds.
     */
    unsigned char* getPixel(int x, int y);

    /**
     * @brief Retrieves a constant pointer to the pixel at the specified (x, y) coordinates.
     * @param x X-coordinate of the pixel.
     * @param y Y-coordinate of the pixel.
     * @return Constant pointer to the pixel data or nullptr if out of bounds.
     */
    const unsigned char* getPixel(int x, int y) const;

    /**
     * @brief Sets the pixel data at the specified (x, y) coordinates.
     * @param x X-coordinate of the pixel.
     * @param y Y-coordinate of the pixel.
     * @param pixel Pointer to the new pixel data.
     */
    void setPixel(int x, int y, const unsigned char* pixel);

    /**
     * @brief Clears the image data by resetting all pixels to zero.
     */
    void setData(const unsigned char* newData, int newWidth, int newHeight, int newChannels);
  
    void clear();

    /**
     * @brief Enables or disables verbose mode for debugging purposes.
     * @param v Set to true to enable verbose mode, false to disable.
     */
    void setVerbose(bool v) { verbose = v; }

private:
    int width;      ///< Image width in pixels.
    int height;     ///< Image height in pixels.
    int channels;   ///< Number of color channels.
    unsigned char* data; ///< Pointer to the image data.
    bool verbose = false; ///< Flag for enabling/disabling verbose mode.
};

#endif // IMAGE_H
