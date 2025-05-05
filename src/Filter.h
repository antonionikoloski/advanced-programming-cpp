#ifndef FILTER_H
#define FILTER_H

#include <string>
#include "Image.h"
#include "Volume.h"

/**
 * @class Filter2D
 * @brief Abstract base class for 2D image filters.
 *
 * Derived classes must implement the `apply` method to apply specific filtering techniques.
 */
class Filter2D {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~Filter2D() = default;

    /**
     * @brief Applies the filter to the given image.
     * @param img The image to apply the filter to.
     */
    virtual void apply(Image& img) = 0;
};

/**
 * @class Filter3D
 * @brief Abstract base class for 3D volume filters.
 *
 * Derived classes must implement the `apply` method to apply specific filtering techniques.
 */
class Filter3D {
public:
    /**
     * @brief Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~Filter3D() = default;

    /**
     * @brief Applies the filter to the given volume.
     * @param vol The volume to apply the filter to.
     */
    virtual void apply(Volume& vol) = 0;
};

// -----------------------------------------------------------------------
// 2D Filter Factory functions
// -----------------------------------------------------------------------

/**
 * @brief Creates a greyscale filter for 2D images.
 * @return Pointer to the greyscale filter instance.
 */
Filter2D* createGreyscaleFilter();

/**
 * @brief Creates a brightness adjustment filter for 2D images.
 * @param value The brightness adjustment value (positive to increase, negative to decrease).
 * @return Pointer to the brightness filter instance.
 */
Filter2D* createBrightnessFilter(int value);

/**
 * @brief Creates a default brightness filter with zero adjustment.
 * @return Pointer to the brightness filter instance.
 */
Filter2D* createBrightnessFilter();

/**
 * @brief Creates a histogram equalization filter for 2D images.
 * @param type The type of histogram equalization to apply (e.g., "global", "local").
 * @return Pointer to the histogram equalization filter instance.
 */
Filter2D* createHistogramEqualisationFilter(const std::string& type);

/**
 * @brief Creates a threshold filter for 2D images.
 * @param thresholdValue The threshold value to apply.
 * @param mode The mode of thresholding (e.g., "binary", "inverse").
 * @return Pointer to the threshold filter instance.
 */
Filter2D* createThresholdFilter(int thresholdValue, const std::string& mode);

/**
 * @brief Creates a salt-and-pepper noise filter for 2D images.
 * @param noiseAmount The amount of noise to introduce (range: 0.0 to 1.0).
 * @return Pointer to the salt-and-pepper filter instance.
 */
Filter2D* createSaltPepperFilter(float noiseAmount);

/**
 * @brief Creates a box blur filter for 2D images.
 * @param kernelSize The size of the blur kernel (must be odd and positive).
 * @return Pointer to the box blur filter instance.
 */
Filter2D* createBoxBlurFilter(int kernelSize);

/**
 * @brief Creates a Gaussian blur filter for 2D images.
 * @param kernelSize The size of the blur kernel (must be odd and positive).
 * @param stdev The standard deviation for the Gaussian distribution.
 * @return Pointer to the Gaussian blur filter instance.
 */
Filter2D* createGaussianBlurFilter(int kernelSize, double stdev = 2.0);

/**
 * @brief Creates a median blur filter for 2D images.
 * @param kernelSize The size of the median filter kernel (must be odd and positive).
 * @return Pointer to the median blur filter instance.
 */
Filter2D* createMedianBlurFilter(int kernelSize);

/**
 * @brief Creates a sharpening filter for 2D images.
 * @return Pointer to the sharpening filter instance.
 */
Filter2D* createSharpenFilter();

/**
 * @brief Creates an edge detection filter for 2D images.
 * @param edgeType The type of edge detection (e.g., "Sobel", "Canny").
 * @return Pointer to the edge detection filter instance.
 */
Filter2D* createEdgeDetectionFilter(const std::string& edgeType);

// -----------------------------------------------------------------------
// 3D Filter Factory functions
// -----------------------------------------------------------------------

/**
 * @brief Creates a Gaussian blur filter for 3D volumes.
 * @param kernelSize The size of the blur kernel (must be odd and positive).
 * @param stdev The standard deviation for the Gaussian distribution.
 * @return Pointer to the Gaussian blur filter instance.
 */
Filter3D* createGaussianBlur3DFilter(int kernelSize, double stdev = 2.0);

/**
 * @brief Creates a median blur filter for 3D volumes.
 * @param kernelSize The size of the median filter kernel (must be odd and positive).
 * @return Pointer to the median blur filter instance.
 */
Filter3D* createMedianBlur3DFilter(int kernelSize);

#endif // FILTER_H
