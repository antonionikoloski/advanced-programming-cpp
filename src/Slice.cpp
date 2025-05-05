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

#include "Slice.h"
#include <iostream>
#include <cstdlib>  // for malloc
#include <cstring>  // for memset

Image Slice::sliceVolume(const Volume& vol, const std::string& plane, int constant) {
    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int ch = vol.getChannels();

    if (plane == "XY") {
        // Slicing the XY plane at z = constant
        // => output 2D image size: width = w, height = h
        // (x in [0..w-1], y in [0..h-1]), 
        // we read from vol.getSlices()[z].getPixel(x, y)
        if (constant < 0 || constant >= d) {
            std::cerr << "[sliceVolume] 'constant' out of range for plane XY.\n";
            return Image();
        }
        // allocate w*h*ch
        unsigned char* outData = (unsigned char*)std::malloc(w * h * ch);
        if (!outData) {
            std::cerr << "[sliceVolume] malloc fail for XY.\n";
            return Image();
        }
        std::memset(outData, 0, w * h * ch);
        Image result(w, h, ch, outData);

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                const unsigned char* voxel = vol.getSlices()[constant].getPixel(x, y);
                if (!voxel) continue; 
                unsigned char* outPix = result.getPixel(x, y);
                for (int c = 0; c < ch; c++) {
                    outPix[c] = voxel[c];
                }
            }
        }
        return result;
    }
    else if (plane == "XZ") {
        // fix y = constant
        // => output width = w, height = d
        // we read (x, constant, z)
        if (constant < 0 || constant >= h) {
            std::cerr << "[sliceVolume] 'constant' out of range for plane XZ.\n";
            return Image();
        }
        unsigned char* outData = (unsigned char*)std::malloc(w * d * ch);
        if (!outData) {
            std::cerr << "[sliceVolume] malloc fail for XZ.\n";
            return Image();
        }
        std::memset(outData, 0, w * d * ch);
        Image result(w, d, ch, outData);

        for (int z = 0; z < d; z++) {
            for (int x = 0; x < w; x++) {
                const unsigned char* voxel = vol.getSlices()[z].getPixel(x, constant);
                if (!voxel) continue;
                // (x, z) in result
                unsigned char* outPix = result.getPixel(x, z);
                for (int c = 0; c < ch; c++) {
                    outPix[c] = voxel[c];
                }
            }
        }
        return result;
    }
    else if (plane == "YZ") {
        // fix x = constant
        // => output width = h, height = d
        // (y in [0..h-1], z in [0..d-1])
        if (constant < 0 || constant >= w) {
            std::cerr << "[sliceVolume] 'constant' out of range for plane YZ.\n";
            return Image();
        }
        unsigned char* outData = (unsigned char*)std::malloc(h * d * ch);
        if (!outData) {
            std::cerr << "[sliceVolume] malloc fail for YZ.\n";
            return Image();
        }
        std::memset(outData, 0, h * d * ch);
        Image result(h, d, ch, outData);

        for (int z = 0; z < d; z++) {
            for (int y = 0; y < h; y++) {
                const unsigned char* voxel = vol.getSlices()[z].getPixel(constant, y);
                if (!voxel) continue;
                // (y, z) in result
                unsigned char* outPix = result.getPixel(y, z);
                for (int c = 0; c < ch; c++) {
                    outPix[c] = voxel[c];
                }
            }
        }
        return result;
    }
    else {
        std::cerr << "[sliceVolume] Unsupported plane type: " << plane << "\n";
        return Image();
    }
}
