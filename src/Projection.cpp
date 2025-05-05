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
#include "Projection.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

// Clamp to [0, 255]
inline unsigned char clamp255(float v) {
    return (v < 0) ? 0 : (v > 255) ? 255 : static_cast<unsigned char>(v + 0.5f);
}

// Normalize an image to [0, 255] per channel
void normalizeImage(Image& img) {
    int w = img.getWidth();
    int h = img.getHeight();
    int ch = img.getChannels();
    unsigned char* data = img.getData();

    for (int c = 0; c < ch; ++c) {
        float minVal = 255.0f, maxVal = 0.0f;
        for (int i = c; i < w * h * ch; i += ch) {
            minVal = std::min(minVal, static_cast<float>(data[i]));
            maxVal = std::max(maxVal, static_cast<float>(data[i]));
        }
        if (maxVal <= minVal) continue; // Avoid division by zero
        float scale = 255.0f / (maxVal - minVal);
        for (int i = c; i < w * h * ch; i += ch) {
            data[i] = clamp255((data[i] - minVal) * scale);
        }
    }
}

/*
 * Performs a Maximum Intensity Projection (MIP) on a 3D volume over a specified Z-range.
 * MIP selects the maximum value along the Z-axis for each (x, y) position, producing a 2D image
 * that highlights the brightest structures in the volume (e.g., bones in a CT scan).
 * Parameters:
 *   vol: The 3D volume to project.
 *   zMin: The starting Z-index (inclusive).
 *   zMax: The ending Z-index (inclusive).
 * Returns: A 2D image containing the MIP result.
 */
static Image mipCore(const Volume& vol, int zMin, int zMax) {
    int w = vol.getWidth();
    int h = vol.getHeight();
    int ch = vol.getChannels();

    zMin = std::max(0, zMin);
    zMax = std::min(vol.getDepth() - 1, zMax);
    if (zMin > zMax) {
        std::cerr << "[MIP] Invalid z range: " << zMin << " to " << zMax << "\n";
        return Image();
    }

    unsigned char* outData = (unsigned char*)std::malloc(w * h * ch);
    if (!outData) {
        std::cerr << "[MIP] Failed to allocate memory.\n";
        return Image();
    }
    std::memcpy(outData, vol.getSlices()[zMin].getData(), w * h * ch);

    Image result(w, h, ch, outData);
    for (int z = zMin + 1; z <= zMax; ++z) {
        const unsigned char* sliceData = vol.getSlices()[z].getData();
        for (int i = 0; i < w * h * ch; ++i) {
            outData[i] = std::max(outData[i], sliceData[i]);
        }
    }
    normalizeImage(result); // Always normalize for visibility
    return result;
}

static Image minipCore(const Volume& vol, int zMin, int zMax) {
    int w = vol.getWidth();
    int h = vol.getHeight();
    int ch = vol.getChannels();

    zMin = std::max(0, zMin);
    zMax = std::min(vol.getDepth() - 1, zMax);
    if (zMin > zMax) {
        std::cerr << "[MinIP] Invalid z range: " << zMin << " to " << zMax << "\n";
        return Image();
    }

    unsigned char* outData = (unsigned char*)std::malloc(w * h * ch);
    if (!outData) {
        std::cerr << "[MinIP] Failed to allocate memory.\n";
        return Image();
    }
    std::memcpy(outData, vol.getSlices()[zMin].getData(), w * h * ch);

    Image result(w, h, ch, outData);
    for (int z = zMin + 1; z <= zMax; ++z) {
        const unsigned char* sliceData = vol.getSlices()[z].getData();
        for (int i = 0; i < w * h * ch; ++i) {
            outData[i] = std::min(outData[i], sliceData[i]);
        }
    }
    normalizeImage(result);
    return result;
}

static Image meanCore(const Volume& vol, int zMin, int zMax) {
    int w = vol.getWidth();
    int h = vol.getHeight();
    int ch = vol.getChannels();

    zMin = std::max(0, zMin);
    zMax = std::min(vol.getDepth() - 1, zMax);
    if (zMin > zMax) {
        std::cerr << "[MeanIP] Invalid z range: " << zMin << " to " << zMax << "\n";
        return Image();
    }

    unsigned char* outData = (unsigned char*)std::malloc(w * h * ch);
    if (!outData) {
        std::cerr << "[MeanIP] Failed to allocate memory.\n";
        return Image();
    }
    Image result(w, h, ch, outData);
    std::vector<float> sums(w * h * ch, 0.0f);
    int count = zMax - zMin + 1;

    for (int z = zMin; z <= zMax; ++z) {
        const unsigned char* sliceData = vol.getSlices()[z].getData();
        for (int i = 0; i < w * h * ch; ++i) {
            sums[i] += sliceData[i];
        }
    }
    float minVal = 255.0f, maxVal = 0.0f;
    for (int i = 0; i < w * h * ch; ++i) {
        float mean = sums[i] / count;
        outData[i] = clamp255(mean);
        minVal = std::min(minVal, mean);
        maxVal = std::max(maxVal, mean);
    }
    std::cout << "[MeanIP] Raw range: min=" << minVal << ", max=" << maxVal << "\n";
    normalizeImage(result); // Normalize to enhance contrast
    return result;
}

static Image medianCore(const Volume& vol, int zMin, int zMax) {
    int w = vol.getWidth();
    int h = vol.getHeight();
    int ch = vol.getChannels();
   // Clamp the Z-range to valid indices.
    zMin = std::max(0, zMin);
    zMax = std::min(vol.getDepth() - 1, zMax);
    if (zMin > zMax) {
        std::cerr << "[MedianIP] Invalid z range: " << zMin << " to " << zMax << "\n";
        return Image();
    }

    unsigned char* outData = (unsigned char*)std::malloc(w * h * ch);
    if (!outData) {
        std::cerr << "[MedianIP] Failed to allocate memory.\n";
        return Image();
    }
    Image result(w, h, ch, outData);
    int count = zMax - zMin + 1;
    std::vector<unsigned char> buffer(count);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < ch; ++c) {
                for (int z = zMin; z <= zMax; ++z) {
                    buffer[z - zMin] = vol.getSlices()[z].getPixel(x, y)[c];
                }
                std::nth_element(buffer.begin(), buffer.begin() + count / 2, buffer.end());
                outData[(y * w + x) * ch + c] = buffer[count / 2];
            }
        }
    }
    normalizeImage(result);
    return result;
}

// Public API
// Public API (corrected)
Image Projection::maximumIntensityProjection(const Volume& vol) {
    return mipCore(vol, 0, vol.getDepth() - 1);
}
Image Projection::minimumIntensityProjection(const Volume& vol) {
    return minipCore(vol, 0, vol.getDepth() - 1);
}
Image Projection::meanIntensityProjection(const Volume& vol) {
    return meanCore(vol, 0, vol.getDepth() - 1);
}
Image Projection::medianIntensityProjection(const Volume& vol) {
    return medianCore(vol, 0, vol.getDepth() - 1);
}

Image Projection::maximumIntensityProjection(const Volume& vol, int zMin, int zMax) {
    return mipCore(vol, zMin - 1, zMax - 1); // Convert 1-based to 0-based
}
Image Projection::minimumIntensityProjection(const Volume& vol, int zMin, int zMax) {
    return minipCore(vol, zMin - 1, zMax - 1);
}
Image Projection::meanIntensityProjection(const Volume& vol, int zMin, int zMax) {
    return meanCore(vol, zMin - 1, zMax - 1);
}
Image Projection::medianIntensityProjection(const Volume& vol, int zMin, int zMax) {
    return medianCore(vol, zMin - 1, zMax - 1);
}
