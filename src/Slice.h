#ifndef SLICE_H
#define SLICE_H

#include <string>
#include "Volume.h"

/**
 * The Slice class is used to slice 3D body data in the XZ or YZ plane, returning a 2D Image.
 * For example:
 * - sliceVolume(vol, "XZ", someY) : fix Y = someY, scan for (x,z)
 * - sliceVolume(vol, "YZ", someX) : fixed X = someX, scan for (y,z)
 */
class Slice {
public:
    /**
     * Slicing function
     * @param vol input 3D body data
     * @param plane "XZ" or "YZ" (can be extended to support "XY")
     * @param constant The coordinate of the corresponding plane (e.g. for "XZ", constant = Y value).
     * @return Returns the generated 2D Image.
     */
    static Image sliceVolume(const Volume& vol, const std::string& plane, int constant);
};

#endif // SLICE_H
