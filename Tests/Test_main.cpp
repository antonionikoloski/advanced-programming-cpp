#include "Test.h"

int main() {
    TestSuite suite;

    // Add all test functions to the suite
    suite.addTest(testGreyscale, "testGreyscale");
    suite.addTest(testBrightness, "testBrightness");
    suite.addTest(testHistogramEqualisation, "testHistogramEqualisation");
    suite.addTest(testThreshold, "testThreshold");
    suite.addTest(testSaltPepperNoise, "testSaltPepperNoise");
    suite.addTest(testBoxBlur, "testBoxBlur");
    suite.addTest(testGaussianBlur, "testGaussianBlur");
    suite.addTest(testMedianBlur, "testMedianBlur");
    suite.addTest(testSharpen, "testSharpen");
    suite.addTest(testEdgeDetection, "testEdgeDetection");

    // Add 3D filter tests
    suite.addTest(testProjectionAll3D, "testProjectionAll3D");
    suite.addTest(testSlice3D, "testSlice3D");
    suite.addTest(testGaussianBlur3D, "testGaussianBlur3D");
    suite.addTest(testMedianBlur3D, "testMedianBlur3D");

    // Run all tests and return the number of failures
    int failures = suite.runAllTests();
    return failures == 0 ? 0 : 1; // Return 0 if all tests pass, 1 if any fail
}