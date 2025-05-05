#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>

/**
 * @class TestSuite
 * @brief A simple test framework to manage and run test functions.
 */
class TestSuite
{
public:
    /**
     * @brief Runs all registered tests.
     * @return The number of failed tests.
     */
    int runAllTests();

    /**
     * @brief Adds a test function to the suite.
     *
     * @param testFunc Pointer to the test function (no parameters and no return value).
     * @param testName Name of the test for identification.
     */
    void addTest(void (*testFunc)(), const std::string &testName);

private:
    /**
     * @struct TestItem
     * @brief Structure representing a test function and its name.
     */
    struct TestItem
    {
        void (*func)();   ///< Pointer to the test function
        std::string name; ///< Name of the test function
    };

    std::vector<TestItem> tests; ///< List of registered test functions.
};

/** @brief Tests greyscale image processing functionality. */
void testGreyscale();

/** @brief Tests brightness adjustment functionality. */
void testBrightness();

/** @brief Tests histogram equalization functionality. */
void testHistogramEqualisation();

/** @brief Tests image thresholding functionality. */
void testThreshold();

/** @brief Tests salt and pepper noise filtering. */
void testSaltPepperNoise();

/** @brief Tests box blur functionality. */
void testBoxBlur();

/** @brief Tests Gaussian blur functionality. */
void testGaussianBlur();

/** @brief Tests median blur functionality. */
void testMedianBlur();

/** @brief Tests image sharpening functionality. */
void testSharpen();

/** @brief Tests edge detection functionality. */
void testEdgeDetection();

// -----3D Filter Tests-----
/** @brief Tests 3D projection functionality. */
void testProjectionAll3D();

/** @brief Tests 3D slicing functionality. */
void testSlice3D();

/** @brief Tests 3D Gaussian blur functionality. */
void testGaussianBlur3D();

/** @brief Tests 3D median blur functionality. */
void testMedianBlur3D();

#endif // TEST_H
