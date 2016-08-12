// Brian Mc George
// MCGBRI004

#include <cmath>
#include <iostream>
#include "include/gaussianblur.hpp"
#include "include/imagefilter.hpp"

namespace mcgbri004 {
/**
 * Applies a gaussian blur to an image
 *
 * @param image     Pointer to image
 * @param imageXLen Length along x-axis of image
 * @param imageYLen Length along y-axis of image
 * @param sigma     Amount of blur
 * @param w         Size of filter
 * @return          Pointer to blurred image
 */
int* applyGaussianBlur(int* image, int imageXLen, int imageYLen, float sigma, int w) {
    float* gaussianMatrix = getGaussianMatrix(sigma, w);

    int* blurredImage = mcgbri004::applyImageFilter(image, imageXLen, imageYLen, gaussianMatrix, w);

    delete[] gaussianMatrix;
    return blurredImage;
}

/**
 * Calculates the values of the gaussian filter
 *
 * @param sigma Amount of blur
 * @param w     Size of filter
 * @return      Pointer to gaussian filter
 */
float* getGaussianMatrix(float sigma, int w) {
    float* gaussianMatrix = new float[w*w];
    float total = 0;
    int midpoint = w / 2;

    for (int y = 0; y < w; ++y) {
        for(int x = 0; x < w; ++x ) {
            int gausianXValue = abs(x - midpoint);
            int gausianYValue = abs(y - midpoint);

            gaussianMatrix[(y * w) + x] = getGaussianValue(sigma, gausianXValue, gausianYValue);
            total += gaussianMatrix[(y * w) + x];
        }
    }
    for (int y = 0; y < w; ++y) {
        for(int x = 0; x < w; ++x ) {
            gaussianMatrix[(y * w) + x] /= total;
        }
    }

    return gaussianMatrix;
}

/**
 * @param sigma Amount of blur
 * @param x     x-value for matrix
 * @param y     y-value for matrix
 * @return      float which is the value of the gaussian at x and y
 */
float getGaussianValue(float sigma, int x, int y) {
    float gaussianValue = (1/(2 * M_PI * pow(sigma, 2))) * exp(-1 *(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
    return gaussianValue;
}
}
