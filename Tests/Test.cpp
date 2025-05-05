#include "Test.h"
#include "Image.h"
#include "Filter.h"
#include <cassert>
#include <iostream>
#include <string>
#include <chrono>

// Helper function: get the pixel value from the Image using getPixel() method.
unsigned char get_pixel(const Image& img, int x, int y, int channel) {
    const unsigned char* p = img.getPixel(x, y);
    return p[channel];
}

// Helper function: check that all pixel values in the image are within [0, 255].
void checkPixelRange(const Image& img) {
    int w = img.getWidth();
    int h = img.getHeight();
    int ch = img.getChannels();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int c = 0; c < ch; ++c) {
                int val = get_pixel(img, x, y, c);
                assert(val >= 0 && val <= 255 && "Pixel value out of range [0,255]");
            }
        }
    }
}

// ------------------- Test functions for 2D Filter -------------------

void testGreyscale() {
    std::cout << "Running testGreyscale..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    // Create greyscale filter using factory function and apply it
    Filter2D* greyscaleFilter = createGreyscaleFilter();
    greyscaleFilter->apply(img);
    delete greyscaleFilter;

    int w = img.getWidth();
    int h = img.getHeight();
    // Check that the image is now 1 channel (greyscale)
    std::cout << img.getChannels() << std::endl;
    assert(img.getChannels() == 1 && "Greyscale should produce a 1-channel image");
    // Check sample pixels: since it's greyscale, we only have 1 channel
    for (int y = 0; y < h; y += 20) {
        for (int x = 0; x < w; x += 20) {
            unsigned char val = get_pixel(img, x, y, 0);
            assert(val >= 0 && val <= 255 && "Greyscale pixel value out of range");
        }
    }
    std::cout << "testGreyscale passed." << std::endl;
}

void testBrightness() {
    std::cout << "Running testBrightness..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* brightnessFilter = createBrightnessFilter(50);
    brightnessFilter->apply(img);
    delete brightnessFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Brightness filter should not change channel count");
    std::cout << "testBrightness passed." << std::endl;
}

void testHistogramEqualisation() {
    std::cout << "Running testHistogramEqualisation..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    // If image is color, convert to greyscale first.
    if (img.getChannels() >= 3) {
        Filter2D* greyscaleFilter = createGreyscaleFilter();
        greyscaleFilter->apply(img);
        delete greyscaleFilter;
        originalChannels = 1;
    }
    Filter2D* histEqFilter = createHistogramEqualisationFilter("default");
    histEqFilter->apply(img);
    delete histEqFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Histogram equalisation should not change channel count");
    std::cout << "testHistogramEqualisation passed." << std::endl;
}

void testThreshold() {
    std::cout << "Running testThreshold..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    // Convert to greyscale for thresholding.
    if (img.getChannels() >= 3) {
        Filter2D* greyscaleFilter = createGreyscaleFilter();
        greyscaleFilter->apply(img);
        delete greyscaleFilter;
        originalChannels = 1;
    }
    Filter2D* thresholdFilter = createThresholdFilter(128, "binary");
    thresholdFilter->apply(img);
    delete thresholdFilter;

    int w = img.getWidth();
    int h = img.getHeight();
    for (int y = 0; y < h; y += 10) {
        for (int x = 0; x < w; x += 10) {
            int val = get_pixel(img, x, y, 0);
            assert((val == 0 || val == 255) && "Threshold: pixel should be 0 or 255");
        }
    }
    assert(img.getChannels() == originalChannels && "Threshold filter should not change channel count");
    std::cout << "testThreshold passed." << std::endl;
}

void testSaltPepperNoise() {
    std::cout << "Running testSaltPepperNoise..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* spFilter = createSaltPepperFilter(5); // 5% noise
    spFilter->apply(img);
    delete spFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Salt and pepper noise filter should not change channel count");
    std::cout << "testSaltPepperNoise passed." << std::endl;
}

void testBoxBlur() {
    std::cout << "Running testBoxBlur..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* boxBlurFilter = createBoxBlurFilter(5);
    boxBlurFilter->apply(img);
    delete boxBlurFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Box blur filter should not change channel count");
    std::cout << "testBoxBlur passed." << std::endl;
}

void testGaussianBlur() {
    std::cout << "Running testGaussianBlur..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* gaussianBlurFilter = createGaussianBlurFilter(5, 1.5);
    gaussianBlurFilter->apply(img);
    delete gaussianBlurFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Gaussian blur filter should not change channel count");
    std::cout << "testGaussianBlur passed." << std::endl;
}

void testMedianBlur() {
    std::cout << "Running testMedianBlur..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* medianBlurFilter = createMedianBlurFilter(5);
    medianBlurFilter->apply(img);
    delete medianBlurFilter;

    checkPixelRange(img);
    assert(img.getChannels() == originalChannels && "Median blur filter should not change channel count");
    std::cout << "testMedianBlur passed." << std::endl;
}

void testSharpen() {
    std::cout << "Running testSharpen..." << std::endl;
    Image img;
    bool loaded = img.load("../Images/gracehopper.png");
    assert(loaded && "Failed to load test image.");

    int originalChannels = img.getChannels();
    Filter2D* sharpenFilter = createSharpenFilter();
    sharpenFilter->apply(img);
    delete sharpenFilter; // Prevents memory leaks

    // Sharpen Test 1: Pixel range Check 
    checkPixelRange(img); // Ensures no pixel values go outside [0,255]

    // Sharpen Test 2: Edge Case Test
    int w = img.getWidth();
    int h = img.getHeight();
    int ch = img.getChannels();
    for (int y = 0; y < h; ++y){
        for (int x = 0; x < w; ++x){
            const unsigned char* pixel = img.getPixel(x,y);
            for (int c = 0; c < ch; ++c){
                assert(pixel[c] >= 0 && pixel[c] <= 255 && "Edge Case Test: Detected extreme pixel value");
            }
        }
    }
    assert(img.getChannels() == originalChannels && "Sharpen filter should not change channel count");
    std::cout << "testSharpen passed (both tests)." << std::endl;
}

void testEdgeDetection() {
    std::cout << "Running testEdgeDetection..." << std::endl;
    std::string types[] = {"Sobel", "Prewitt", "Scharr", "RobertsCross"};
    for (const auto& type : types) {
        Image img;
        bool loaded = img.load("../Images/gracehopper.png");
        assert(loaded && "Failed to load test image.");

        int originalChannels = img.getChannels();
        Filter2D* edgeFilter = createEdgeDetectionFilter(type);
        edgeFilter->apply(img);
        delete edgeFilter;

        if (originalChannels >= 3) {
            originalChannels = 1; // Convert to grayscale before edge detection, the output should be 1 channel
        }

        checkPixelRange(img);
        assert(img.getChannels() == originalChannels && "Edge detection filter should produce correct channel count");
        std::cout << "Edge detection (" << type << ") passed." << std::endl;
    }
}

// ------------------- Test functions for 3D Filter -------------------
#include "Volume.h"
#include "Projection.h"
#include "Slice.h"

//--------------------------------------------------------------------------
// 3D Projection：MIP, MinIP, meanAIP, medianAIP
//--------------------------------------------------------------------------
void testProjectionAll3D() {
    std::cout << "Running testProjectionAll3D..." << std::endl;

    // Load vol data
    Volume vol;
    bool loaded = vol.load("../Scans/TestVolume/vol");
    assert(loaded && "Failed to load volume from Scans/TestVolume/vol");

    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int ch = vol.getChannels();

    std::cout << "Volume loaded: " << w << " x " << h << " x " << d
        << ", channels = " << ch << std::endl;
    // 32 "vol" images 
    assert(d == 32 && "Expected 32 slices in the volume.");

    int zMin = 1, zMax = d; // All slices

    // 1. MIP test
    {
        Image mip = Projection::maximumIntensityProjection(vol, zMin, zMax);
        assert(mip.getWidth() == w && "MIP: output width incorrect");
        assert(mip.getHeight() == h && "MIP: output height incorrect");
        assert(mip.getChannels() == ch && "MIP: output channels incorrect");
        std::cout << "MIP test passed: " << mip.getWidth() << "x"
            << mip.getHeight() << ", channels=" << mip.getChannels() << std::endl;
    }

    // 2. MinIP test
    {
        Image minip = Projection::minimumIntensityProjection(vol, zMin, zMax);
        assert(minip.getWidth() == w && "MinIP: output width incorrect");
        assert(minip.getHeight() == h && "MinIP: output height incorrect");
        assert(minip.getChannels() == ch && "MinIP: output channels incorrect");
        std::cout << "MinIP test passed: " << minip.getWidth() << "x"
            << minip.getHeight() << ", channels=" << minip.getChannels() << std::endl;
    }

    // 3. meanAIP test
    {
        Image meanAIP = Projection::meanIntensityProjection(vol, zMin, zMax);
        assert(meanAIP.getWidth() == w && "meanAIP: output width incorrect");
        assert(meanAIP.getHeight() == h && "meanAIP: output height incorrect");
        assert(meanAIP.getChannels() == ch && "meanAIP: output channels incorrect");
        std::cout << "meanAIP test passed: " << meanAIP.getWidth() << "x"
            << meanAIP.getHeight() << ", channels=" << meanAIP.getChannels() << std::endl;
    }

    // 4. medianAIP test
    {
        Image medianAIP = Projection::medianIntensityProjection(vol, zMin, zMax);
        assert(medianAIP.getWidth() == w && "medianAIP: output width incorrect");
        assert(medianAIP.getHeight() == h && "medianAIP: output height incorrect");
        assert(medianAIP.getChannels() == ch && "medianAIP: output channels incorrect");
        std::cout << "medianAIP test passed: " << medianAIP.getWidth() << "x"
            << medianAIP.getHeight() << ", channels=" << medianAIP.getChannels() << std::endl;
    }

    std::cout << "testProjectionAll3D passed." << std::endl;
}

// ------------------- Test functions for 3D Slice -------------------
void testSlice3D() {
    std::cout << "Running testSlice3D..." << std::endl;

    // load "vol" images
    Volume vol;
    bool loaded = vol.load("../Scans/TestVolume/vol");
    assert(loaded && "Failed to load volume from Scans/TestVolume/vol");

    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int ch = vol.getChannels();
    std::cout << "Volume loaded: "
        << w << "x" << h << "x" << d
        << ", channels=" << ch << std::endl;

    assert(d == 32 && "Expected 32 slices in the volume.");

    // --- Test XZ Planar Slicing --- 
    // Fix the y-coordinate (e.g. middle position y = h/2)
    // the output image should be width = w, height = d (number of slices)
    int yConst = h / 2;
    Image sliceXZ = Slice::sliceVolume(vol, "XZ", yConst);
    assert(sliceXZ.getWidth() == w && "Slice XZ: width incorrect");
    assert(sliceXZ.getHeight() == d && "Slice XZ: height should equal volume depth");
    assert(sliceXZ.getChannels() == ch && "Slice XZ: channels incorrect");
    std::cout << "Slice XZ test passed: Dimensions = "
        << sliceXZ.getWidth() << "x" << sliceXZ.getHeight()
        << ", channels = " << sliceXZ.getChannels() << std::endl;

    // --- Testing YZ plane slices ---
    // Fix the x-coordinate (e.g. middle position x = w/2)
    // the output image should be width = h, height = d
    int xConst = w / 2;
    Image sliceYZ = Slice::sliceVolume(vol, "YZ", xConst);
    assert(sliceYZ.getWidth() == h && "Slice YZ: width should equal original slice height");
    assert(sliceYZ.getHeight() == d && "Slice YZ: height should equal volume depth");
    assert(sliceYZ.getChannels() == ch && "Slice YZ: channels incorrect");
    std::cout << "Slice YZ test passed: Dimensions = "
        << sliceYZ.getWidth() << "x" << sliceYZ.getHeight()
        << ", channels = " << sliceYZ.getChannels() << std::endl;

    std::cout << "testSlice3D passed." << std::endl;
}

//--------------------------------------------------------------------------
// 3D Filter Test: GaussianBlur3D and MedianBlur3D
//--------------------------------------------------------------------------
void testGaussianBlur3D() {
    std::cout << "Running testGaussianBlur3D..." << std::endl;

    Volume vol;
    bool loaded = vol.load("../Scans/TestVolume/vol");
    assert(loaded && "Failed to load volume for GaussianBlur3D test");

    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int ch = vol.getChannels();

    // Create a 3D Gaussian Blur filter with parameters
    // such as kernelSize = 3, stdev = 1.5
    Filter3D* gaussian3D = createGaussianBlur3DFilter(3, 1.5);
    gaussian3D->apply(vol);
    delete gaussian3D;

    // Check that the size of Volume and the number of channels remain the same.
    assert(vol.getWidth() == w && "GaussianBlur3D: width changed");
    assert(vol.getHeight() == h && "GaussianBlur3D: height changed");
    assert(vol.getDepth() == d && "GaussianBlur3D: depth changed");
    assert(vol.getChannels() == ch && "GaussianBlur3D: channels changed");

    // Check that the pixel values in each slice are in the range [0,255]
    for (int i = 0; i < d; i++) {
        Image& slice = vol.getSlices()[i];
        int sw = slice.getWidth();
        int sh = slice.getHeight();
        int sch = slice.getChannels();
        for (int y = 0; y < sh; y++) {
            for (int x = 0; x < sw; x++) {
                const unsigned char* pix = slice.getPixel(x, y);
                for (int c = 0; c < sch; c++) {
                    int val = pix[c];
                    assert(val >= 0 && val <= 255 && "GaussianBlur3D: pixel value out of range");
                }
            }
        }
    }
    std::cout << "GaussianBlur3D test passed." << std::endl;
}

void testMedianBlur3D() {
    std::cout << "Running testMedianBlur3D..." << std::endl;

    Volume vol;
    bool loaded = vol.load("../Scans/TestVolume/vol");
    assert(loaded && "Failed to load volume for MedianBlur3D test");

    int w = vol.getWidth();
    int h = vol.getHeight();
    int d = vol.getDepth();
    int ch = vol.getChannels();

    // Creates a 3D Median Blur filter with parameters such as kernelSize = 3
    Filter3D* median3D = createMedianBlur3DFilter(3);
    median3D->apply(vol);
    delete median3D;

    // Check that the size of Volume and the number of channels remain the same.
    assert(vol.getWidth() == w && "MedianBlur3D: width changed");
    assert(vol.getHeight() == h && "MedianBlur3D: height changed");
    assert(vol.getDepth() == d && "MedianBlur3D: depth changed");
    assert(vol.getChannels() == ch && "MedianBlur3D: channels changed");

    // Check that the pixel values in each slice are in the range [0,255]
    for (int i = 0; i < d; i++) {
        Image& slice = vol.getSlices()[i];
        int sw = slice.getWidth();
        int sh = slice.getHeight();
        int sch = slice.getChannels();
        for (int y = 0; y < sh; y++) {
            for (int x = 0; x < sw; x++) {
                const unsigned char* pix = slice.getPixel(x, y);
                for (int c = 0; c < sch; c++) {
                    int val = pix[c];
                    assert(val >= 0 && val <= 255 && "MedianBlur3D: pixel value out of range");
                }
            }
        }
    }
    std::cout << "MedianBlur3D test passed." << std::endl;
}

// Implementation of TestSuite class: manages and runs all tests.
int TestSuite::runAllTests() {
    int failCount = 0;
    for (const auto& test : tests) {
        std::cout << "------------------------" << std::endl;
        std::cout << "Running test: " << test.name << std::endl;

		// start timer
        auto start = std::chrono::high_resolution_clock::now();
        try {
            test.func();
            auto end = std::chrono::high_resolution_clock::now();
            // Calculation time (in milliseconds)
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cout << test.name << " PASSED. (Time: "
                << elapsed.count() << " ms)" << std::endl;
        }
        catch (...) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::cerr << test.name << " FAILED. (Time: "
                << elapsed.count() << " ms)" << std::endl;
            failCount++;
        }
    }
    std::cout << "Total tests: " << tests.size()
        << ", Failures: " << failCount << std::endl;
    return failCount;
}

void TestSuite::addTest(void (*testFunc)(), const std::string& testName) {
    tests.push_back({ testFunc, testName });
}