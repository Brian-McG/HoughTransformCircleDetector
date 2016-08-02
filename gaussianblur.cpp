// Brian Mc George
// MCGBRI004

#include <cmath>
#include <iostream>
#include "./gaussianblur.hpp"
#include "./imagefilter.hpp"

namespace mcgbri004 {

int* applyGausianBlur(int* image, int imageXLen, int imageYLen, float sigma, int w) {
    float* gausianMatrix = getGausianMatrix(sigma, w);

    int* blurredImage = mcgbri004::applyImageFilter(image, imageXLen, imageYLen, gausianMatrix, w);

    delete gausianMatrix;
    return blurredImage;
}


float* getGausianMatrix(float sigma, int w) {
    float* gausianMatrix = new float[w*w];
    float total = 0;
    int midpoint = w / 2;
    std::cout << "midpoint: " << midpoint << std::endl;

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

float getGausianValue(float sigma, int x, int y) {
    float gausianValue = (1/(2 * M_PI * pow(sigma, 2))) * exp(-1 *(pow(x, 2) + pow(y, 2)) / (2 * pow(sigma, 2)));
    std::cout << (1/(2 * M_PI * pow(sigma, 2)))  << " " << gausianValue << " " << y << " " << x << std::endl;
    return gausianValue;
}
}
