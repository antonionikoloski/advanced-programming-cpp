#ifndef PROJECTION_H
#define PROJECTION_H

#include "Volume.h"
// doxygen comments formatted with ChatGPT
/**
 * @class Projection
 * @brief Provides methods for generating 2D projections from 3D volume data.
 *
 * This class offers multiple projection techniques such as maximum, minimum,
 * mean, and median intensity projections. Overloaded versions allow for
 * specifying a custom Z-range for focused projections.
 */
class Projection {
public:
    /**
     * @brief Generates a Maximum Intensity Projection (MIP) from the entire volume.
     * @param vol The input 3D volume data.
     * @return A 2D image representing the maximum intensity projection.
     */
    static Image maximumIntensityProjection(const Volume& vol);

    /**
     * @brief Generates a Minimum Intensity Projection (MinIP) from the entire volume.
     * @param vol The input 3D volume data.
     * @return A 2D image representing the minimum intensity projection.
     */
    static Image minimumIntensityProjection(const Volume& vol);

    /**
     * @brief Generates a Mean Intensity Projection from the entire volume.
     * @param vol The input 3D volume data.
     * @return A 2D image representing the mean intensity projection.
     */
    static Image meanIntensityProjection(const Volume& vol);

    /**
     * @brief Generates a Median Intensity Projection from the entire volume.
     * @param vol The input 3D volume data.
     * @return A 2D image representing the median intensity projection.
     */
    static Image medianIntensityProjection(const Volume& vol);

    // -----------------------------------------------------------------------
    // Overloaded versions for specified Z-range
    // -----------------------------------------------------------------------

    /**
     * @brief Generates a Maximum Intensity Projection (MIP) within a specified Z-range.
     * @param vol The input 3D volume data.
     * @param zMin The minimum Z index (inclusive).
     * @param zMax The maximum Z index (inclusive).
     * @return A 2D image representing the maximum intensity projection for the given range.
     */
    static Image maximumIntensityProjection(const Volume& vol, int zMin, int zMax);

    /**
     * @brief Generates a Minimum Intensity Projection (MinIP) within a specified Z-range.
     * @param vol The input 3D volume data.
     * @param zMin The minimum Z index (inclusive).
     * @param zMax The maximum Z index (inclusive).
     * @return A 2D image representing the minimum intensity projection for the given range.
     */
    static Image minimumIntensityProjection(const Volume& vol, int zMin, int zMax);

    /**
     * @brief Generates a Mean Intensity Projection within a specified Z-range.
     * @param vol The input 3D volume data.
     * @param zMin The minimum Z index (inclusive).
     * @param zMax The maximum Z index (inclusive).
     * @return A 2D image representing the mean intensity projection for the given range.
     */
    static Image meanIntensityProjection(const Volume& vol, int zMin, int zMax);

    /**
     * @brief Generates a Median Intensity Projection within a specified Z-range.
     * @param vol The input 3D volume data.
     * @param zMin The minimum Z index (inclusive).
     * @param zMax The maximum Z index (inclusive).
     * @return A 2D image representing the median intensity projection for the given range.
     */
    static Image medianIntensityProjection(const Volume& vol, int zMin, int zMax);
};

#endif // PROJECTION_H
