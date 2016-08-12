// Brian Mc George
// MCGBRI004

#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H
#define _USE_MATH_DEFINES
#include <math.h>

namespace mcgbri004 {
float* getGaussianMatrix(float sigma, int w);
float getGaussianValue(float sigma, int x, int y);
int* applyGaussianBlur(int* image, int imageXLen, int imageYLen, float sigma, int w);
}
#endif // GAUSSIANBLUR_H
