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

 #ifndef COLORCONVERTER_HPP
 #define COLORCONVERTER_HPP
 
 #include <cmath>
 #include <algorithm>
 
 /**
  * @namespace ColorConverter
  * @brief Provides functions for converting colors between RGB, HSV, and HSL color spaces.
  */
 namespace ColorConverter
 {
 
     /**
      * @brief Converts RGB color to HSV color.
      * 
      * @param r Red component (0-255)
      * @param g Green component (0-255)
      * @param b Blue component (0-255)
      * @param[out] h Hue (0-360)
      * @param[out] s Saturation (0-1)
      * @param[out] v Value (0-1)
      */
     inline void rgbToHSV(unsigned char r, unsigned char g, unsigned char b,
                          float &h, float &s, float &v)
     {
         float fr = r / 255.0f;
         float fg = g / 255.0f;
         float fb = b / 255.0f;
 
         float maxVal = std::max({fr, fg, fb});
         float minVal = std::min({fr, fg, fb});
         float delta = maxVal - minVal;
 
         v = maxVal;
 
         if (delta < 1e-6f)
         {
             h = 0.0f;
             s = 0.0f;
             return;
         }
 
         s = delta / maxVal;
 
         if (maxVal == fr)
             h = 60.0f * fmod(((fg - fb) / delta), 6.0f);
         else if (maxVal == fg)
             h = 60.0f * (((fb - fr) / delta) + 2.0f);
         else
             h = 60.0f * (((fr - fg) / delta) + 4.0f);
 
         if (h < 0.0f)
             h += 360.0f;
     }
 
     /**
      * @brief Converts HSV color to RGB color.
      * 
      * @param h Hue (0-360)
      * @param s Saturation (0-1)
      * @param v Value (0-1)
      * @param[out] r Red component (0-255)
      * @param[out] g Green component (0-255)
      * @param[out] b Blue component (0-255)
      */
     inline void hsvToRGB(float h, float s, float v,
                          unsigned char &r, unsigned char &g, unsigned char &b)
     {
         float c = v * s;
         float hh = fmod(h / 60.0f, 6.0f);
         float x = c * (1.0f - fabs(fmod(hh, 2.0f) - 1.0f));
         float m = v - c;
 
         float fr, fg, fb;
         switch (static_cast<int>(hh))
         {
         case 0: fr = c; fg = x; fb = 0; break;
         case 1: fr = x; fg = c; fb = 0; break;
         case 2: fr = 0; fg = c; fb = x; break;
         case 3: fr = 0; fg = x; fb = c; break;
         case 4: fr = x; fg = 0; fb = c; break;
         default: fr = c; fg = 0; fb = x; break;
         }
 
         r = static_cast<unsigned char>(std::round((fr + m) * 255.0f));
         g = static_cast<unsigned char>(std::round((fg + m) * 255.0f));
         b = static_cast<unsigned char>(std::round((fb + m) * 255.0f));
     }
 
     /**
      * @brief Converts RGB color to HSL color.
      * 
      * @param r Red component (0-255)
      * @param g Green component (0-255)
      * @param b Blue component (0-255)
      * @param[out] h Hue (0-360)
      * @param[out] s Saturation (0-1)
      * @param[out] l Lightness (0-1)
      */
     inline void rgbToHSL(unsigned char r, unsigned char g, unsigned char b,
                          float &h, float &s, float &l)
     {
         float fr = r / 255.0f;
         float fg = g / 255.0f;
         float fb = b / 255.0f;
 
         float maxVal = std::max({fr, fg, fb});
         float minVal = std::min({fr, fg, fb});
         float delta = maxVal - minVal;
 
         l = (maxVal + minVal) * 0.5f;
 
         if (delta < 1e-6f)
         {
             h = 0.0f;
             s = 0.0f;
             return;
         }
 
         s = (l < 0.5f) ? delta / (maxVal + minVal) : delta / (2.0f - maxVal - minVal);
 
         if (maxVal == fr)
             h = 60.0f * fmod(((fg - fb) / delta), 6.0f);
         else if (maxVal == fg)
             h = 60.0f * (((fb - fr) / delta) + 2.0f);
         else
             h = 60.0f * (((fr - fg) / delta) + 4.0f);
 
         if (h < 0.0f)
             h += 360.0f;
     }
 
     /**
      * @brief Converts HSL color to RGB color.
      * 
      * @param h Hue (0-360)
      * @param s Saturation (0-1)
      * @param l Lightness (0-1)
      * @param[out] r Red component (0-255)
      * @param[out] g Green component (0-255)
      * @param[out] b Blue component (0-255)
      */
     inline void hslToRGB(float h, float s, float l,
                          unsigned char &r, unsigned char &g, unsigned char &b)
     {
         float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
         float hh = fmod(h / 60.0f, 6.0f);
         float x = c * (1.0f - fabs(fmod(hh, 2.0f) - 1.0f));
         float m = l - c * 0.5f;
 
         float fr, fg, fb;
         switch (static_cast<int>(hh))
         {
         case 0: fr = c; fg = x; fb = 0; break;
         case 1: fr = x; fg = c; fb = 0; break;
         case 2: fr = 0; fg = c; fb = x; break;
         case 3: fr = 0; fg = x; fb = c; break;
         case 4: fr = x; fg = 0; fb = c; break;
         default: fr = c; fg = 0; fb = x; break;
         }
 
         r = static_cast<unsigned char>(std::round((fr + m) * 255.0f));
         g = static_cast<unsigned char>(std::round((fg + m) * 255.0f));
         b = static_cast<unsigned char>(std::round((fb + m) * 255.0f));
     }
 
 } // namespace ColorConverter
 
 #endif // COLORCONVERTER_HPP
 
