/*
 * Group Name: Dijkstra
 * Members:
 *   - Antonio Nikoloski (GitHub:esemsc-an1224)
 *   - Tyana Tshiota (GitHub:esemsc-tnt24)
 *   - Yue Zigi  (GitHub: esemsc-zy424)
 *   - Yang Aijia (GitHub:esemsc-ay424)
 *   - Zhou Xiaorui (GitHub:esemsc-zx24)
 *   - Zhang Zewei (GitHub:esemsc-zz724)
 *   - Azam Sazina (GitHub:esemsc-as1224)
 */

 #include "Filter.h"
 #include "ColorConverter.hpp"
 #include <iostream>
 #include <vector>
 #include <cmath>
 #include <ctime>
 #include <algorithm>
 #include <cstdlib> // for rand()
 #include <string>
 #include <random>
 #include <cstring>
 #ifndef M_PI
 #define M_PI 3.14159265358979323846
 #endif
 
 /*
  * Enum to define edge handling modes for filters that access pixels outside image boundaries.
  * This is used in filters like BoxBlurFilter2D, GaussianBlurFilter2D, and MedianBlurFilter2D to
  * handle out-of-bounds coordinates. We included multiple modes to support different edge handling
  * strategies, as specified in the project requirements.
  */
 enum class EdgeMode
 {
     Extend,
     Reflect,
     Constant,
     Wrap
 };
 /*
  * Anonymous namespace to encapsulate helper functions that are only used within this file.
  * This keeps the functions private to Filter.cpp, avoiding naming conflicts and improving encapsulation.
  */
 namespace
 {
 
     // This is a helper function for quickSelect, used in MedianBlurFilter2D and MedianBlurFilter3D
     //  to find the median value in a neighborhood of pixels.
     int partition(std::vector<int> &arr, int left, int right, int pivotIndex)
     {
         int pivotValue = arr[pivotIndex];
         std::swap(arr[pivotIndex], arr[right]); // Move pivot to end
         int storeIndex = left;
         for (int i = left; i < right; i++)
         {
             if (arr[i] < pivotValue)
             {
                 std::swap(arr[i], arr[storeIndex]);
                 storeIndex++;
             }
         }
         std::swap(arr[storeIndex], arr[right]); // Move pivot to its final place
         return storeIndex;
     }
 
     /*
      * Recursive function to select the k-th smallest element in a vector using the QuickSelect algorithm.
      * This is used in MedianBlurFilter2D and MedianBlurFilter3D to find the median pixel value in a neighborhood.
      * We chose QuickSelect because it has an average-case time complexity of O(n), which is faster than sorting
      * the entire array (O(n log n)) when we only need the median.
      */
     int quickSelect(std::vector<int> &arr, int left, int right, int k)
     {
         if (left == right)
         {
             return arr[left];
         }
         if (left > right)
         {
             throw std::invalid_argument("Invalid range in quickSelect: left > right");
         }
 
         int pivotIndex = left + rand() % (right - left + 1);
         pivotIndex = partition(arr, left, right, pivotIndex);
 
         if (k == pivotIndex)
         {
             return arr[k];
         }
         else if (k < pivotIndex)
         {
             return quickSelect(arr, left, pivotIndex - 1, k);
         }
         else
         {
             return quickSelect(arr, pivotIndex + 1, right, k);
         }
     }
 
     // Overloaded quickSelect for convenience
     int quickSelect(std::vector<int> &arr, int k)
     {
         return quickSelect(arr, 0, (int)arr.size() - 1, k);
     }
     // Helper function to create a 2D filter
     static void rgbToHsv(unsigned char r, unsigned char g, unsigned char b,
                          float &H, float &S, float &V)
     {
         float fr = r / 255.0f;
         float fg = g / 255.0f;
         float fb = b / 255.0f;
         float maxC = std::max({fr, fg, fb});
         float minC = std::min({fr, fg, fb});
         float delta = maxC - minC;
 
         V = maxC; // value
         if (maxC == 0.0f)
         {
             S = 0.0f;
             H = 0.0f;
             return;
         }
         else
         {
             S = delta / maxC;
         }
 
         if (delta < 1e-6)
         {
             H = 0.0f;
         }
         else
         {
             if (maxC == fr)
             {
                 H = 60.0f * ((fg - fb) / delta);
                 if (H < 0.0f)
                     H += 360.0f;
             }
             else if (maxC == fg)
             {
                 H = 60.0f * ((fb - fr) / delta + 2.0f);
             }
             else
             { // maxC == fb
                 H = 60.0f * ((fr - fg) / delta + 4.0f);
             }
         }
         if (H < 0.0f)
         {
             H += 360.0f;
         }
         if (H >= 360.0f)
         {
             H -= 360.0f;
         }
     }
 
     static void hsvToRgb(float H, float S, float V,
                          unsigned char &r, unsigned char &g, unsigned char &b)
     {
         // hue is [0..360], S and V are [0..1].
         // from standard HSV->RGB logic
         if (S <= 1e-6)
         {
             // gray
             unsigned char val = (unsigned char)(V * 255.0f + 0.5f);
             r = val;
             g = val;
             b = val;
             return;
         }
         float hh = H / 60.0f;
         int i = (int)hh;
         float ff = hh - i;
         float p = V * (1.0f - S);
         float q = V * (1.0f - (S * ff));
         float t = V * (1.0f - (S * (1.0f - ff)));
 
         float rF, gF, bF; // in [0..1]
         switch (i)
         {
         case 0:
             rF = V;
             gF = t;
             bF = p;
             break;
         case 1:
             rF = q;
             gF = V;
             bF = p;
             break;
         case 2:
             rF = p;
             gF = V;
             bF = t;
             break;
         case 3:
             rF = p;
             gF = q;
             bF = V;
             break;
         case 4:
             rF = t;
             gF = p;
             bF = V;
             break;
         default:
             rF = V;
             gF = p;
             bF = q;
             break;
         }
         r = (unsigned char)(rF * 255.0f + 0.5f);
         g = (unsigned char)(gF * 255.0f + 0.5f);
         b = (unsigned char)(bF * 255.0f + 0.5f);
     }
     static int getIndex(int coord, int size, EdgeMode mode)
     {
         switch (mode)
         {
         case EdgeMode::Extend:
         default:
             // clamp
             return std::clamp(coord, 0, size - 1);
 
         case EdgeMode::Reflect:
             while (coord < 0 || coord >= size)
             {
                 if (coord < 0)
                     coord = -coord - 1;
                 else if (coord >= size)
                     coord = 2 * size - coord - 1;
             }
             return coord;
 
         case EdgeMode::Constant:
             if (coord < 0 || coord >= size)
             {
                 return -1;
             }
             return coord;
 
         case EdgeMode::Wrap:
             coord = coord % size;
             if (coord < 0)
                 coord += size;
             return coord;
         }
     }
 
 } // end anonymous namespace
 
 //=============================================================================
 //                           2D FILTERS
 //=============================================================================
 
 /*
  * 1) GreyscaleFilter2D
  * This class converts a color image to grayscale using the luminance formula.
  * We implemented this filter to support converting RGB images to grayscale, which is often a
  * preprocessing step for other filters like EdgeDetectionFilter2D.
  */
 class GreyscaleFilter2D : public Filter2D
 {
 public:
     void apply(Image &img) override
     {
         const int width = img.getWidth();
         const int height = img.getHeight();
         const int channels = img.getChannels();
 
         if (channels < 3)
         {
             throw std::invalid_argument(
                 "GreyscaleFilter requires at least 3 channels (got " +
                 std::to_string(channels) + ")");
         }
 
         const int totalPixels = width * height;
 
         // Allocate n buffer for single-channel grayscale data
         std::vector<unsigned char> grayscaleData(totalPixels);
 
         constexpr float red_coeff = 0.2126f;
         constexpr float green_coeff = 0.7152f;
         constexpr float blue_coeff = 0.0722f;
 
         unsigned char *__restrict data = img.getData(); // Original data
 
         // Greyscale conversion
         for (int i = 0; i < totalPixels; ++i)
         {
             const int idx = i * channels;
 
             // Greyscale calculation with rounding
             const float gray_float =
                 red_coeff * data[idx] +
                 green_coeff * data[idx + 1] +
                 blue_coeff * data[idx + 2];
 
             const auto gray = static_cast<unsigned char>(gray_float + 0.5f);
             grayscaleData[i] = gray;
         }
 
         // Sets the image data to new single-channel data
         img.setData(grayscaleData.data(), width, height, 1); // 1 channel
     }
 };
 
 /*
  * 2) BrightnessFilter2D
  * This class adjusts the brightness of an image, either manually (with a specified value) or
  * automatically (based on the average intensity). We implemented this filter to allow users to
  * enhance the visibility of images by making them brighter or darker.
  */
 class BrightnessFilter2D : public Filter2D
 {
 public:
     // Manual mode constructor
     explicit BrightnessFilter2D(int value) : mode_(Manual), value_(value)
     {
         if (value < -255 || value > 255)
         {
             throw std::invalid_argument("Brightness value must be between -255 and 255");
         }
     }
 
     /*
      * Applies the brightness filter to the given image.
      * Depending on the mode (Manual or Auto), it either applies a fixed brightness adjustment
      * or computes an adjustment based on the image's average intensity.
      */
     BrightnessFilter2D() : mode_(Auto) {}
 
     void apply(Image &img) override
     {
         const int width = img.getWidth();
         const int height = img.getHeight();
         const int channels = img.getChannels();
         unsigned char *data = img.getData();
 
         if (width == 0 || height == 0)
             return;
 
         if (mode_ == Manual)
         {
             applyManual(data, width, height, channels);
         }
         else
         {
             applyAuto(data, width, height, channels);
         }
     }
 
 private:
     enum Mode
     {
         Manual,
         Auto
     };
     Mode mode_;
     int value_;
 
     // Optimized Manual Mode with Branchless Clamping
     void applyManual(unsigned char *data, int w, int h, int ch) const
     {
         const size_t total = w * h * ch;
 
         if (value_ == 0)
             return; // No processing needed if brightness adjustment is zero
 
         for (size_t i = 0; i < total; ++i)
         {
             const int newValue = data[i] + value_;
             data[i] = (newValue < 0) ? 0 : (newValue > 255) ? 255
                                                             : newValue; // Branchless clamping
         }
     }
 
     /*
      * Applies brightness adjustment in Auto mode.
      * This method calculates the average intensity of the image and adjusts the brightness
      * to bring the average closer to 128 (midpoint of the intensity range).
      * Parameters:
      *   data: The image data to modify.
      *   w, h: The width and height of the image.
      *   ch: The number of channels in the image.
      */
     void applyAuto(unsigned char *data, int w, int h, int ch) const
     {
         const size_t totalPixels = w * h;
         double sum = 0.0;
 
         // Calculate average intensity for color channels only
         const int colorChannels = (ch == 2 || ch == 4) ? ch - 1 : ch;
 
         for (size_t i = 0; i < totalPixels; ++i)
         {
             const int idx = i * ch;
             for (int c = 0; c < colorChannels; ++c)
             {
                 sum += data[idx + c];
             }
         }
 
         const double average = sum / (totalPixels * colorChannels);
         const int delta = static_cast<int>(128 - average + 0.5); // Rounding
 
         // Apply to color channels only with optimized clamping
         for (size_t i = 0; i < totalPixels; ++i)
         {
             const int idx = i * ch;
             for (int c = 0; c < colorChannels; ++c)
             {
                 int temp = data[idx + c] + delta;
                 data[idx + c] = (temp < 0) ? 0 : (temp > 255) ? 255
                                                               : temp; // Branchless clamping
             }
         }
     }
 };
 
 /*
  * 3) HistogramEqualisationFilter2D
  * This class applies histogram equalization to an image in either HSV or HSL color space.
  * We implemented this filter to enhance the contrast of images by redistributing the intensity
  * values, making details more visible in both bright and dark areas.
  */
 class HistogramEqualisationFilter2D : public Filter2D
 {
 public:
     /**
      * Constructor for HistogramEqualisationFilter2D
      *
      * @param type - The color space mode: "HSV" or "HSL".
      */
     explicit HistogramEqualisationFilter2D(const std::string &type)
         : type_(type) {}
 
     /**
      * Applies the histogram equalization filter to the given image.
      *
      * @param img - Reference to the image object.
      */
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
 
         // Return immediately if the image has invalid dimensions or channels.
         if (w == 0 || h == 0 || ch == 0)
             return;
 
         unsigned char *data = img.getData();
 
         // 1) HSV Equalization
         if ((ch == 3 || ch == 4) && type_ == "HSV")
         {
             eqColorHSV(data, w, h, ch);
         }
         // 2) HSL Equalization
         else if ((ch == 3 || ch == 4) && type_ == "HSL")
         {
             eqColorHSL(data, w, h, ch);
         }
     }
 
 private:
     std::string type_; // Type of equalization ("HSV" or "HSL")
 
     // -------------------------------------------------------------------------
     // (A) HSV-Based Histogram Equalization
     // -------------------------------------------------------------------------
     void eqColorHSV(unsigned char *data, int w, int h, int ch) const
     {
         const int total = w * h;
         std::vector<unsigned char> vChannel(total);
 
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, V;
             ColorConverter::rgbToHSV(r, g, b, H, S, V);
 
             vChannel[i] = static_cast<unsigned char>(
                 std::clamp(static_cast<int>(std::round(V * 255.0f)), 0, 255));
         }
 
         histogramEqualize(vChannel);
 
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, V;
             ColorConverter::rgbToHSV(r, g, b, H, S, V);
 
             float newV = vChannel[i] / 255.0f;
 
             unsigned char rr, gg, bb;
             ColorConverter::hsvToRGB(H, S, newV, rr, gg, bb);
 
             data[idx] = rr;
             data[idx + 1] = gg;
             data[idx + 2] = bb;
         }
     }
 
     // -------------------------------------------------------------------------
     // (B) HSL-Based Histogram Equalization
     // -------------------------------------------------------------------------
     void eqColorHSL(unsigned char *data, int w, int h, int ch) const
     {
         const int total = w * h;
         std::vector<unsigned char> lChannel(total);
 
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, L;
             ColorConverter::rgbToHSL(r, g, b, H, S, L);
 
             lChannel[i] = static_cast<unsigned char>(
                 std::clamp(static_cast<int>(std::round(L * 255.0f)), 0, 255));
         }
 
         histogramEqualize(lChannel);
 
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, L;
             ColorConverter::rgbToHSL(r, g, b, H, S, L);
 
             float newL = lChannel[i] / 255.0f;
 
             unsigned char rr, gg, bb;
             ColorConverter::hslToRGB(H, S, newL, rr, gg, bb);
 
             data[idx] = rr;
             data[idx + 1] = gg;
             data[idx + 2] = bb;
         }
     }
 
     // -------------------------------------------------------------------------
     // Utility: Histogram Equalization Algorithm
     // -------------------------------------------------------------------------
     void histogramEqualize(std::vector<unsigned char> &channel) const
     {
         int totalPixels = channel.size();
 
         std::vector<int> hist(256, 0);
         for (int i = 0; i < totalPixels; i++)
         {
             hist[channel[i]]++;
         }
 
         std::vector<int> cdf(256, 0);
         cdf[0] = hist[0];
         for (int i = 1; i < 256; i++)
         {
             cdf[i] = cdf[i - 1] + hist[i];
         }
 
         int cdf_min = 0;
         for (int i = 0; i < 256; i++)
         {
             if (cdf[i] != 0)
             {
                 cdf_min = cdf[i];
                 break;
             }
         }
 
         for (int i = 0; i < totalPixels; i++)
         {
             int oldVal = channel[i];
             int newVal = static_cast<int>(std::round(
                 (cdf[oldVal] - cdf_min) * 255.0 / (totalPixels - cdf_min)));
             channel[i] = static_cast<unsigned char>(std::clamp(newVal, 0, 255));
         }
     }
 };
 
 /*
  * 4) ThresholdFilter2D
  * This class applies a threshold filter to an image, supporting Grayscale, HSV, and HSL modes.
  * We implemented this filter to segment images by converting pixel values to either 0 (black) or
  * 255 (white) based on a threshold, which is useful for tasks like binary image creation.
  */
 class ThresholdFilter2D : public Filter2D
 {
 public:
     ThresholdFilter2D(int thresholdValue, const std::string &mode)
         : thresholdValue_(std::clamp(thresholdValue, 0, 255)), mode_(mode) {}
 
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
 
         if (w == 0 || h == 0 || ch == 0)
             return;
 
         unsigned char *data = img.getData();
 
         if (ch == 1)
         {
             thresholdGray(data, w, h); // Single-channel grayscale images
         }
         else if ((ch == 3 || ch == 4) && mode_ == "HSV")
         {
             thresholdColorHSV(data, w, h, ch);
         }
         else if ((ch == 3 || ch == 4) && mode_ == "HSL")
         {
             thresholdColorHSL(data, w, h, ch);
         }
         else
         {
             thresholdDirect(data, w, h, ch); // RGB or unknown mode fallback
         }
     }
 
 private:
     int thresholdValue_;
     std::string mode_;
 
     // (A) Grayscale Threshold
     void thresholdGray(unsigned char *data, int w, int h) const
     {
         const int total = w * h;
         for (int i = 0; i < total; i++)
         {
             data[i] = (data[i] < thresholdValue_) ? 0 : 255;
         }
     }
 
     // (B) HSV-Based Threshold
     void thresholdColorHSV(unsigned char *data, int w, int h, int ch) const
     {
         const int total = w * h;
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, V;
             ColorConverter::rgbToHSV(r, g, b, H, S, V);
 
             float thresholdNorm = std::round(thresholdValue_ / 255.0f * 1000.0f) / 1000.0f;
 
             if (V <= 0.0f)
             { // Pure black
                 r = g = b = 0;
             }
             else if (V >= 1.0f)
             { // Pure white
                 r = g = b = 255;
             }
             else if (V < thresholdNorm)
             {
                 r = g = b = 0; // Set to black
             }
             else
             {
                 r = g = b = 255; // Set to white
             }
 
             data[idx] = r;
             data[idx + 1] = g;
             data[idx + 2] = b;
 
             if (ch == 4)
                 data[idx + 3] = 255; // Discard alpha values
         }
     }
 
     // (C) HSL-Based Threshold
     void thresholdColorHSL(unsigned char *data, int w, int h, int ch) const
     {
         const int total = w * h;
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             unsigned char r = data[idx];
             unsigned char g = data[idx + 1];
             unsigned char b = data[idx + 2];
 
             float H, S, L;
             ColorConverter::rgbToHSL(r, g, b, H, S, L);
 
             unsigned char gray = (L < thresholdValue_ / 255.0f) ? 0 : 255;
             data[idx] = data[idx + 1] = data[idx + 2] = gray;
 
             if (ch == 4)
                 data[idx + 3] = 255; // Discard alpha values
         }
     }
 
     // (D) Direct Threshold for RGB Images
     void thresholdDirect(unsigned char *data, int w, int h, int ch) const
     {
         const int total = w * h;
         for (int i = 0; i < total; i++)
         {
             int idx = i * ch;
             for (int c = 0; c < ch; c++)
             {
                 if (c != 3)
                 { // Ignore alpha channel
                     data[idx + c] = (data[idx + c] < thresholdValue_) ? 0 : 255;
                 }
             }
         }
     }
 };
 /*
  * 5) SaltPepperFilter2D
  * This class adds salt-and-pepper noise to an image by randomly setting pixels to 0 (black) or
  * 255 (white). We implemented this filter to simulate noise in images, which can be used to test
  * the effectiveness of noise reduction filters like MedianBlurFilter2D.
  */
 class SaltPepperFilter2D : public Filter2D
 {
 public:
     explicit SaltPepperFilter2D(float noisePercent)
         : noisePercent_(noisePercent)
     {
         if (noisePercent_ < 0.0f)
             noisePercent_ = 0.0f;
         if (noisePercent_ > 100.0f)
             noisePercent_ = 100.0f;
     }
 
     void apply(Image &img) override
     {
         const int w = img.getWidth();
         const int h = img.getHeight();
         const int ch = img.getChannels();
 
         if (w <= 0 || h <= 0 || ch <= 0)
             return;
 
         unsigned char *data = img.getData();
         const int totalPixels = w * h;
         const int numNoisy = static_cast<int>(std::round(totalPixels * (noisePercent_ / 100.0f)));
 
         // Uses seperate logic for low-noise cases
         if (noisePercent_ <= 5.0f)
         {
             applyFastNoise(data, totalPixels, ch, numNoisy);
         }
         else
         {
             applyAccurateNoise(data, totalPixels, ch, numNoisy);
         }
     }
 
 private:
     float noisePercent_;
 
     // Method for Handling Low Noise (≤ 5%)
     void applyFastNoise(unsigned char *data, int totalPixels, int ch, int numNoisy) const
     {
         std::srand(static_cast<unsigned int>(std::time(nullptr)));
 
         for (int i = 0; i < numNoisy; ++i)
         {
             int idx = (std::rand() % totalPixels) * ch;
             unsigned char noiseValue = (std::rand() % 2 == 0) ? 0 : 255;
 
             for (int c = 0; c < (ch == 4 ? 3 : ch); ++c)
             {
                 data[idx + c] = noiseValue; // Skip alpha channel if present
             }
         }
     }
 
     // Method for Higher Noise (> 5%)
     void applyAccurateNoise(unsigned char *data, int totalPixels, int ch, int numNoisy) const
     {
         std::vector<int> indices(totalPixels);
         for (int i = 0; i < totalPixels; i++)
         {
             indices[i] = i;
         }
 
         std::random_device rd;
         std::mt19937 g(rd());
         std::shuffle(indices.begin(), indices.end(), g);
 
         std::uniform_int_distribution<int> coinFlip(0, 1);
         const int channelsToAffect = (ch == 4 ? 3 : ch);
 
         for (int i = 0; i < numNoisy; i++)
         {
             int pixelIndex = indices[i];
             int base = pixelIndex * ch;
 
             unsigned char val = (coinFlip(g) == 0) ? 0 : 255;
 
             for (int c = 0; c < channelsToAffect; c++)
             {
                 data[base + c] = val;
             }
         }
     }
 };
 /*
  * 6) BoxBlurFilter2D
  * This class applies a box blur (mean filter) to an image using a square kernel.
  * We implemented this filter to smooth images by averaging pixel values in a neighborhood,
  * which helps reduce noise and fine details.
  */
 class BoxBlurFilter2D : public Filter2D
 {
 public:
     BoxBlurFilter2D(int kernelSize, EdgeMode edgeMode = EdgeMode::Extend)
         : kernelSize_(kernelSize), edgeMode_(edgeMode) {}
 
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
         if (w == 0 || h == 0 || ch == 0)
             return;
 
         unsigned char *data = img.getData();
         // Copy original
         std::vector<unsigned char> temp(data, data + (w * h * ch));
 
         int k = kernelSize_ / 2;
         for (int y = 0; y < h; ++y)
         {
             for (int x = 0; x < w; ++x)
             {
                 for (int c = 0; c < ch; ++c)
                 {
                     int sum = 0;
                     int count = 0;
                     for (int dy = -k; dy <= k; ++dy)
                     {
                         int ny = getIndex(y + dy, h, edgeMode_);
                         if (ny < 0)
                         {
                             // means out-of-bounds if using Constant => skip or add 0
                             continue;
                         }
                         for (int dx = -k; dx <= k; ++dx)
                         {
                             int nx = getIndex(x + dx, w, edgeMode_);
                             if (nx < 0)
                             {
                                 // out-of-bounds => skip or 0
                                 continue;
                             }
                             sum += temp[(ny * w + nx) * ch + c];
                             count++;
                         }
                     }
                     if (count > 0)
                     {
                         data[(y * w + x) * ch + c] = static_cast<unsigned char>(sum / count);
                     }
                     else
                     {
                         data[(y * w + x) * ch + c] = 0; // if everything out-of-bounds
                     }
                 }
             }
         }
     }
 
 private:
     int kernelSize_;
     EdgeMode edgeMode_;
 };
 /*
  * 7) GaussianBlurFilter2D
  * This class applies a Gaussian blur to an image using a Gaussian kernel.
  * We implemented this filter to smooth images with a weighted average, where weights follow a
  * Gaussian distribution, providing a more natural blur compared to a box blur.
  */
 class GaussianBlurFilter2D : public Filter2D
 {
 public:
     GaussianBlurFilter2D(int kernelSize, double stdev = 2.0, EdgeMode edgeMode = EdgeMode::Extend)
         : kernelSize_(kernelSize), stdev_(stdev), edgeMode_(edgeMode) {}
 
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
         if (w == 0 || h == 0 || ch == 0)
             return;
 
         unsigned char *data = img.getData();
         std::vector<unsigned char> temp(data, data + (w * h * ch));
 
         // Build the kernel
         int k = kernelSize_ / 2;
         std::vector<std::vector<double>> kernel(kernelSize_, std::vector<double>(kernelSize_, 0.0));
 
         double sigma2 = stdev_ * stdev_;
         double coeff = 1.0 / (2.0 * M_PI * sigma2);
         double sum = 0.0;
 
         for (int dy = -k; dy <= k; dy++)
         {
             for (int dx = -k; dx <= k; dx++)
             {
                 double val = coeff * std::exp(-(dx * dx + dy * dy) / (2.0 * sigma2));
                 kernel[dy + k][dx + k] = val;
                 sum += val;
             }
         }
         // normalize
         for (int yy = 0; yy < kernelSize_; yy++)
         {
             for (int xx = 0; xx < kernelSize_; xx++)
             {
                 kernel[yy][xx] /= sum;
             }
         }
 
         // convolve
         for (int y = 0; y < h; y++)
         {
             for (int x = 0; x < w; x++)
             {
                 for (int c = 0; c < ch; c++)
                 {
                     double accum = 0.0;
                     double weightSum = 0.0;
 
                     for (int dy = -k; dy <= k; dy++)
                     {
                         int ny = getIndex(y + dy, h, edgeMode_);
                         int kernelY = dy + k;
 
                         if (ny < 0)
                         {
                             // constant => skip or treat as 0
                             continue;
                         }
                         for (int dx = -k; dx <= k; dx++)
                         {
                             int nx = getIndex(x + dx, w, edgeMode_);
                             int kernelX = dx + k;
 
                             if (nx < 0)
                             {
                                 continue;
                             }
                             double wght = kernel[kernelY][kernelX];
                             accum += temp[(ny * w + nx) * ch + c] * wght;
                             weightSum += wght;
                         }
                     }
                     // If we skip out-of-bounds, weightSum might < 1.0 for Constant mode
                     // we could re-normalize or not. Typically for Extend or Reflect, it won't be an issue
                     if (weightSum > 1e-6)
                     {
                         accum /= weightSum;
                     }
 
                     int val = static_cast<int>(std::round(accum));
                     val = std::clamp(val, 0, 255);
                     data[(y * w + x) * ch + c] = static_cast<unsigned char>(val);
                 }
             }
         }
     }
 
 private:
     int kernelSize_;
     double stdev_;
     EdgeMode edgeMode_;
 };
 
 /**
  * 8) MedianBlurFilter2D
  */
 class MedianBlurFilter2D : public Filter2D
 {
 public:
     MedianBlurFilter2D(int kernelSize, EdgeMode edgeMode = EdgeMode::Extend)
         : kernelSize_(kernelSize), edgeMode_(edgeMode) {}
 
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
         if (w == 0 || h == 0 || ch == 0)
             return;
 
         unsigned char *data = img.getData();
         std::vector<unsigned char> temp(data, data + (w * h * ch));
 
         int k = kernelSize_ / 2;
 
         for (int y = 0; y < h; y++)
         {
             for (int x = 0; x < w; x++)
             {
                 for (int c = 0; c < ch; c++)
                 {
                     std::vector<int> vals;
                     vals.reserve(kernelSize_ * kernelSize_);
 
                     for (int dy = -k; dy <= k; dy++)
                     {
                         int ny = getIndex(y + dy, h, edgeMode_);
                         if (ny < 0)
                         {
                             // skip if constant
                             continue;
                         }
                         for (int dx = -k; dx <= k; dx++)
                         {
                             int nx = getIndex(x + dx, w, edgeMode_);
                             if (nx < 0)
                             {
                                 continue;
                             }
                             vals.push_back(temp[(ny * w + nx) * ch + c]);
                         }
                     }
                     // find the median
                     if (!vals.empty())
                     {
                         int m = quickSelect(vals, (int)vals.size() / 2);
                         data[(y * w + x) * ch + c] = (unsigned char)m;
                     }
                     else
                     {
                         data[(y * w + x) * ch + c] = 0; // if every neighbor was out-of-bounds
                     }
                 }
             }
         }
     }
 
 private:
     int kernelSize_;
     EdgeMode edgeMode_;
 };
 
 /*
  * 9) SharpenFilter2D
  * This class applies a sharpening filter to an image using a Laplacian kernel.
  * We implemented this filter to enhance edges and details in an image by amplifying differences
  * between a pixel and its neighbors.
  */
 class SharpenFilter2D : public Filter2D
 {
 public:
     void apply(Image &img) override
     {
         int w = img.getWidth();
         int h = img.getHeight();
         int ch = img.getChannels();
         unsigned char *data = img.getData();
         std::vector<unsigned char> temp(data, data + w * h * ch);
 
         // Laplacian kernel:
         //  0 -1  0
         // -1  4 -1
         //  0 -1  0
         for (int y = 1; y < h - 1; y++)
         {
             for (int x = 1; x < w - 1; x++)
             {
                 for (int c = 0; c < ch; c++)
                 {
                     int idx = (y * w + x) * ch + c;
                     int lap = 4 * temp[idx] - temp[((y - 1) * w + x) * ch + c] - temp[((y + 1) * w + x) * ch + c] - temp[(y * w + (x - 1)) * ch + c] - temp[(y * w + (x + 1)) * ch + c];
                     int val = temp[idx] + lap;
                     val = std::clamp(val, 0, 255);
                     data[idx] = static_cast<unsigned char>(val);
                 }
             }
         }
     }
 };
 
 /*
  * 10) EdgeDetectionFilter2D
  * This class applies an edge detection filter to an image using various methods (Sobel, Prewitt, Scharr, Roberts).
  * We implemented this filter to detect edges in images, which is useful for tasks like object detection or segmentation.
  */
 class EdgeDetectionFilter2D : public Filter2D
 {
 public:
     explicit EdgeDetectionFilter2D(const std::string &edgeType)
         : edgeType_(edgeType)
     {
         // optionally sanitize edgeType_ (make uppercase, etc.)
     }
 
     void apply(Image &img) override
     {
         // 1) If color => convert to greyscale
         if (img.getChannels() >= 3)
         {
             GreyscaleFilter2D().apply(img);
         }
         const int w = img.getWidth();
         const int h = img.getHeight();
         int ch = img.getChannels();
         if (w == 0 || h == 0 || ch == 0)
         {
             return;
         }
         // After Greyscale, we might have 1 channel (or 3 if code didn't truly remove alpha).
         // Typically, you'll now have ch=1 or 3. We'll assume 1 or 3.
 
         // 2) Copy the data so we can refer to original while writing new
         unsigned char *data = img.getData();
         std::vector<unsigned char> temp(data, data + w * h * ch);
 
         // 3) Dispatch based on edgeType_
         if (edgeType_ == "Sobel")
         {
             // Gx, Gy for Sobel
             int Gx[3][3] = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
             int Gy[3][3] = {{-1, -2, -1},
                             {0, 0, 0},
                             {1, 2, 1}};
             applyEdgeKernels(temp, data, w, h, ch, Gx, Gy, 3);
         }
         else if (edgeType_ == "Prewitt")
         {
             int Gx[3][3] = {{-1, 0, 1},
                             {-1, 0, 1},
                             {-1, 0, 1}};
             int Gy[3][3] = {{-1, -1, -1},
                             {0, 0, 0},
                             {1, 1, 1}};
             applyEdgeKernels(temp, data, w, h, ch, Gx, Gy, 3);
         }
         else if (edgeType_ == "Scharr")
         {
             int Gx[3][3] = {{-3, 0, 3},
                             {-10, 0, 10},
                             {-3, 0, 3}};
             int Gy[3][3] = {{-3, -10, -3},
                             {0, 0, 0},
                             {3, 10, 3}};
             applyEdgeKernels(temp, data, w, h, ch, Gx, Gy, 3);
         }
         else if (edgeType_ == "RobertsCross" || edgeType_ == "Roberts")
         {
             applyRobertsCross(temp, data, w, h, ch);
         }
         else
         {
             std::cerr << "[EdgeDetectionFilter2D] Unknown edge type: " << edgeType_ << "\n";
         }
     }
 
 private:
     std::string edgeType_;
 
     /**
      * Helper: apply Gx, Gy of size NxN (typically 3x3).
      * Then magnitude = sqrt(sumX^2 + sumY^2).
      */
     template <int N>
     void applyEdgeKernels(const std::vector<unsigned char> &src,
                           unsigned char *dst,
                           int w, int h, int ch,
                           const int (&Gx)[N][N],
                           const int (&Gy)[N][N],
                           int kernelSize) const
     {
         // Typically kernelSize = 3 for Sobel, Prewitt, Scharr
         int k = kernelSize / 2;
         for (int y = 0; y < h; y++)
         {
             for (int x = 0; x < w; x++)
             {
                 // We'll compute sumX, sumY from Gx, Gy
                 int sumX = 0;
                 int sumY = 0;
                 for (int i = -k; i <= k; i++)
                 {
                     for (int j = -k; j <= k; j++)
                     {
                         int ny = std::clamp(y + i, 0, h - 1);
                         int nx = std::clamp(x + j, 0, w - 1);
                         // For grayscale or channel=1, we read from [idx].
                         // If ch==3 still after greyscale, we store the same value in R,G,B
                         // We'll assume we just read the first channel for the gradient
                         int val = src[(ny * w + nx) * ch];
                         // offset in kernel
                         int ky = i + k;
                         int kx = j + k;
                         sumX += Gx[ky][kx] * val;
                         sumY += Gy[ky][kx] * val;
                     }
                 }
                 int mag = (int)(std::sqrt((double)(sumX * sumX + sumY * sumY)));
                 mag = std::clamp(mag, 0, 255);
 
                 int idx = (y * w + x) * ch;
                 // set R=G=B to magnitude if ch>=3, or just channel 0 if ch==1
                 dst[idx] = (unsigned char)mag;
                 if (ch >= 3)
                 {
                     dst[idx + 1] = (unsigned char)mag;
                     dst[idx + 2] = (unsigned char)mag;
                 }
                 // if ch==4 alpha, we might skip or keep alpha as is
             }
         }
     }
 
     /**
      * Helper for Roberts Cross. It's a 2x2 kernel:
      *   G1 = [1 0; 0 -1], G2 = [0 1; -1 0].
      */
     void applyRobertsCross(const std::vector<unsigned char> &src,
                            unsigned char *dst,
                            int w, int h, int ch) const
     {
         // For each pixel, we apply G1, G2
         //  G1 => a-d,  G2 => b-c
         // We can do a 2x2 approach or replicate the formula you had.
         for (int y = 0; y < h; y++)
         {
             for (int x = 0; x < w; x++)
             {
                 // a = (y,x)
                 // b = (y, x+1)
                 // c = (y+1, x)
                 // d = (y+1, x+1)
                 int aIdx = y * w + x;
                 // clamp for b, c, d
                 int bIdx = (x + 1 < w) ? (y * w + (x + 1)) : aIdx;
                 int cIdx = (y + 1 < h) ? ((y + 1) * w + x) : aIdx;
                 int dIdx = (x + 1 < w && y + 1 < h) ? ((y + 1) * w + (x + 1)) : aIdx;
 
                 // read from channel 0
                 int a = src[aIdx * ch];
                 int b = src[bIdx * ch];
                 int c = src[cIdx * ch];
                 int d = src[dIdx * ch];
 
                 int G1 = a - d;
                 int G2 = b - c;
                 int mag = (int)std::sqrt((double)(G1 * G1 + G2 * G2));
                 mag = std::clamp(mag, 0, 255);
 
                 int idx = (y * w + x) * ch;
                 dst[idx] = (unsigned char)mag;
                 if (ch >= 3)
                 {
                     dst[idx + 1] = (unsigned char)mag;
                     dst[idx + 2] = (unsigned char)mag;
                 }
             }
         }
     }
 };
 
 // ----------------------------------------------------------------------
 // 2D Filter Factory Implementations
 // ----------------------------------------------------------------------
 
 Filter2D *createGreyscaleFilter()
 {
     return new GreyscaleFilter2D();
 }
 Filter2D *createBrightnessFilter(int value)
 {
     return new BrightnessFilter2D(value);
 }
 Filter2D *createBrightnessFilter()
 {
     return new BrightnessFilter2D(); // the auto mode
 }
 
 Filter2D *createHistogramEqualisationFilter(const std::string &type)
 {
     return new HistogramEqualisationFilter2D(type);
 }
 Filter2D *createThresholdFilter(int thresholdValue, const std::string &mode)
 {
     return new ThresholdFilter2D(thresholdValue, mode);
 }
 Filter2D *createSaltPepperFilter(float noiseAmount)
 {
     return new SaltPepperFilter2D(noiseAmount);
 }
 Filter2D *createBoxBlurFilter(int kernelSize)
 {
     return new BoxBlurFilter2D(kernelSize);
 }
 Filter2D *createGaussianBlurFilter(int kernelSize, double stdev)
 {
     return new GaussianBlurFilter2D(kernelSize, stdev);
 }
 Filter2D *createMedianBlurFilter(int kernelSize)
 {
     return new MedianBlurFilter2D(kernelSize);
 }
 Filter2D *createSharpenFilter()
 {
     return new SharpenFilter2D();
 }
 Filter2D *createEdgeDetectionFilter(const std::string &edgeType)
 {
     return new EdgeDetectionFilter2D(edgeType);
 }
 
 //=============================================================================
 //                           3D FILTERS
 //=============================================================================
 
 /*
  * GaussianBlurFilter3D
  * This class applies a Gaussian blur to a 3D volume using a separable kernel.
  * We implemented this filter to smooth 3D volumes (e.g., stacks of medical images) by applying
  * a Gaussian blur in three passes (X, Y, Z), which is more efficient than a full 3D convolution.
  */
 class GaussianBlurFilter3D : public Filter3D
 {
 public:
     GaussianBlurFilter3D(int kernelSize, double stdev = 2.0)
         : kernelSize_(kernelSize), stdev_(stdev)
     {
         if (kernelSize_ < 1)
         {
             kernelSize_ = 1;
         }
         if (stdev_ <= 0.0)
         {
             stdev_ = 2.0;
         }
     }
 
     void apply(Volume &vol) override
     {
         int w = vol.getWidth();
         int h = vol.getHeight();
         int d = vol.getDepth();
         int ch = vol.getChannels();
 
         if (w == 0 || h == 0 || d == 0 || ch == 0)
         {
             std::cerr << "[GaussianBlur3DSeparable] Volume is empty or invalid.\n";
             return;
         }
 
         // 1) Build a 1D Gaussian kernel of length kernelSize_
         build1DKernel();
 
         // 2) We will handle data in a single large buffer for convenience
         //    This is w*h*d*ch in size
         //    We'll do 3 passes: X pass, Y pass, Z pass
         size_t totalSize = (size_t)w * h * d * ch;
 
         // Let's copy the original volume data into bufferA
         std::vector<unsigned char> bufferA(totalSize);
         {
             size_t offset = 0;
             for (int z = 0; z < d; z++)
             {
                 unsigned char *sliceData = vol.getSlices()[z].getData();
                 memcpy(bufferA.data() + offset, sliceData, w * h * ch);
                 offset += (w * h * ch);
             }
         }
 
         // We'll create bufferB for intermediate results
         std::vector<unsigned char> bufferB(totalSize);
 
         // 3) Pass along X dimension
         passX(bufferA, bufferB, w, h, d, ch);
         // Now bufferB has the X-blurred result
 
         // 4) Pass along Y dimension
         passY(bufferB, bufferA, w, h, d, ch);
         // Now bufferA has the X+Y-blurred result
 
         // 5) Pass along Z dimension
         passZ(bufferA, bufferB, w, h, d, ch);
         // Now bufferB has fully blurred volume
 
         // 6) Write bufferB back into the Volume slices
         {
             size_t offset = 0;
             for (int z = 0; z < d; z++)
             {
                 unsigned char *sliceData = vol.getSlices()[z].getData();
                 memcpy(sliceData, bufferB.data() + offset, w * h * ch);
                 offset += (w * h * ch);
             }
         }
     }
 
 private:
     int kernelSize_;
     double stdev_;
     std::vector<double> kernel1D_; // store 1D Gaussian kernel
 
     /**
      * Build 1D Gaussian kernel of length = kernelSize_.
      * center = kernelSize_/2
      */
     void build1DKernel()
     {
         kernel1D_.resize(kernelSize_);
         int half = kernelSize_ / 2;
         double sigma2 = stdev_ * stdev_;
         double coeff = 1.0 / (std::sqrt(2.0 * M_PI) * stdev_);
         double sum = 0.0;
 
         // build
         for (int i = 0; i < kernelSize_; i++)
         {
             int x = i - half;
             double val = coeff * std::exp(-(x * x) / (2.0 * sigma2));
             kernel1D_[i] = val;
             sum += val;
         }
         // normalize
         for (int i = 0; i < kernelSize_; i++)
         {
             kernel1D_[i] /= sum;
         }
     }
 
     /**
      * Pass along the X dimension:
      *   For each z in [0..d-1], y in [0..h-1], convolve row in x
      * input -> output
      */
     void passX(const std::vector<unsigned char> &input,
                std::vector<unsigned char> &output,
                int w, int h, int d, int ch) const
     {
         int half = kernelSize_ / 2;
         size_t idx = 0;
         // We'll iterate z, y, x
         for (int z = 0; z < d; z++)
         {
             for (int y = 0; y < h; y++)
             {
                 for (int x = 0; x < w; x++)
                 {
                     // For each channel
                     for (int c = 0; c < ch; c++)
                     {
                         double accum = 0.0;
                         // convolve
                         for (int k = 0; k < kernelSize_; k++)
                         {
                             int xx = x + (k - half);
                             xx = std::clamp(xx, 0, w - 1);
                             size_t inPos = (z * (size_t)(w * h) + y * w + xx) * ch + c;
                             accum += input[inPos] * kernel1D_[k];
                         }
                         int val = (int)std::round(accum);
                         val = std::clamp(val, 0, 255);
 
                         // write to output
                         size_t outPos = (z * (size_t)(w * h) + y * w + x) * ch + c;
                         output[outPos] = (unsigned char)val;
                     }
                 }
             }
         }
     }
 
     /**
      * Pass along the Y dimension:
      *   For each z in [0..d-1], x in [0..w-1], we convolve columns in y
      */
     void passY(const std::vector<unsigned char> &input,
                std::vector<unsigned char> &output,
                int w, int h, int d, int ch) const
     {
         int half = kernelSize_ / 2;
         for (int z = 0; z < d; z++)
         {
             for (int x = 0; x < w; x++)
             {
                 for (int y = 0; y < h; y++)
                 {
                     for (int c = 0; c < ch; c++)
                     {
                         double accum = 0.0;
                         for (int k = 0; k < kernelSize_; k++)
                         {
                             int yy = y + (k - half);
                             yy = std::clamp(yy, 0, h - 1);
                             size_t inPos = (z * (size_t)(w * h) + yy * w + x) * ch + c;
                             accum += input[inPos] * kernel1D_[k];
                         }
                         int val = (int)std::round(accum);
                         val = std::clamp(val, 0, 255);
 
                         size_t outPos = (z * (size_t)(w * h) + y * w + x) * ch + c;
                         output[outPos] = (unsigned char)val;
                     }
                 }
             }
         }
     }
 
     /**
      * Pass along the Z dimension:
      *   For each y in [0..h-1], x in [0..w-1], we convolve in z
      */
     void passZ(const std::vector<unsigned char> &input,
                std::vector<unsigned char> &output,
                int w, int h, int d, int ch) const
     {
         int half = kernelSize_ / 2;
         // We'll keep x,y as outer loops, since we do a 1D pass in z
         for (int y = 0; y < h; y++)
         {
             for (int x = 0; x < w; x++)
             {
                 for (int z = 0; z < d; z++)
                 {
                     for (int c = 0; c < ch; c++)
                     {
                         double accum = 0.0;
                         for (int k = 0; k < kernelSize_; k++)
                         {
                             int zz = z + (k - half);
                             zz = std::clamp(zz, 0, d - 1);
                             size_t inPos = (zz * (size_t)(w * h) + y * w + x) * ch + c;
                             accum += input[inPos] * kernel1D_[k];
                         }
                         int val = (int)std::round(accum);
                         val = std::clamp(val, 0, 255);
 
                         size_t outPos = (z * (size_t)(w * h) + y * w + x) * ch + c;
                         output[outPos] = (unsigned char)val;
                     }
                 }
             }
         }
     }
 };
 
 /**
  * MedianBlurFilter3D
  */
 class MedianBlurFilter3D : public Filter3D
 {
 public:
     explicit MedianBlurFilter3D(int kernelSize)
         : kernelSize_(kernelSize) {}
 
     void apply(Volume &vol) override
     {
         int w = vol.getWidth();
         int h = vol.getHeight();
         int d = vol.getDepth();
         int ch = vol.getChannels();
         if (w == 0 || h == 0 || d == 0)
         {
             std::cerr << "[medianBlur3D] Volume is empty or invalid.\n";
             return;
         }
 
         // Backup data
         std::vector<unsigned char> original;
         original.reserve(w * h * d * ch);
         for (int z = 0; z < d; z++)
         {
             unsigned char *sliceData = vol.getSlices()[z].getData();
             original.insert(original.end(), sliceData, sliceData + (w * h * ch));
         }
 
         int k = kernelSize_ / 2;
         for (int z = 0; z < d; z++)
         {
             for (int y = 0; y < h; y++)
             {
                 for (int x = 0; x < w; x++)
                 {
                     for (int c = 0; c < ch; c++)
                     {
                         std::vector<int> neighbors;
                         neighbors.reserve(kernelSize_ * kernelSize_ * kernelSize_);
                         for (int zz = -k; zz <= k; zz++)
                         {
                             int zpos = std::clamp(z + zz, 0, d - 1);
                             for (int yy = -k; yy <= k; yy++)
                             {
                                 int ypos = std::clamp(y + yy, 0, h - 1);
                                 for (int xx = -k; xx <= k; xx++)
                                 {
                                     int xpos = std::clamp(x + xx, 0, w - 1);
                                     int idx3D = (zpos * (w * h) + ypos * w + xpos) * ch + c;
                                     neighbors.push_back(original[idx3D]);
                                 }
                             }
                         }
                         // median
                         int medianVal = quickSelect(neighbors, (int)neighbors.size() / 2);
                         vol.getSlices()[z].getData()[(y * w + x) * ch + c] = (unsigned char)medianVal;
                     }
                 }
             }
         }
     }
 
 private:
     int kernelSize_;
 };
 
 // ----------------------------------------------------------------------
 // 3D Factory Implementations
 // ----------------------------------------------------------------------
 Filter3D *createGaussianBlur3DFilter(int kernelSize, double stdev)
 {
     return new GaussianBlurFilter3D(kernelSize, stdev);
 }
 Filter3D *createMedianBlur3DFilter(int kernelSize)
 {
     return new MedianBlurFilter3D(kernelSize);
 }
 