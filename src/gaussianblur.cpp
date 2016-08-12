// Brian Mc George
// MCGBRI004

#include <cmath>
#include <iostream>
#include "include/gaussianblur.hpp"
#include "include/imagefilter.hpp"

namespace mcgbri004 {
/**
 * Applies a gausian blur to an image
 *
 * @param image     Pointer to image
 * @param imageXLen Length along x-axis of image
 * @param imageYLen Length along y-axis of image
 * @param sigma     Amount of blur
 * @param w         Size of filter
 * @return          Pointer to blurred image
 */
int* applyGausianBlur(int* image, int imageXLen, int imageYLen, float sigma, int w) {
    float* gausianMatrix = getGausianMatrix(sigma, w);

    int* blurredImage = mcgbri004::applyImageFilter(image, imageXLen, imageYLen, gausianMatrix, w);

    delete[] gausianMatrix;
    return blurredImage;
}

/**
 * Calculates the values of the gausian filter
 *
 * @param sigma Amount of blur
 * @param w     Size of filter
 * @return      Pointer to gausian filter
 */
float* getGausianMatrix(float sigma, int w) {
    float* gausianMatrix = new float[w*w];
    float total = 0;
    int midpoint = w / 2;

    for (int y = 0; y < w; ++y) {
        for(int x = 0; x < w; ++x ) {
            int gausianXValue = abs(x - midpoint);
            int gausianYValue = abs(y - midpoint);

            gausianMatrix[(y * w) + x] = getGausianValue(sigma, gausianXValue, gausianYValue);
            total += gausianMatrix[(y * w) + x];
        }
    }
    for (int y = 0; y < w; ++y) {
        for(int x = 0; x < w; ++x ) {
            gausianMatrix[(y * w) + x] /= total;
        }
    }

    return gausianMatrix;
}

/**
 * @param sigma Amount of blur
 * @param x     x-value for matrix
 * @param y     y-value for matrix
 * @return      float which is the value of the gausian at x and y
 */
float getGausianValue(float sigma, int x, int y) {
    float gausianValue = (1/(2 * M_PI * pow(sigma, 2))) * exp(-1 *(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
    return gausianValue;
}
}
