// Brian Mc George
// MCGBRI004

#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H
#define _USE_MATH_DEFINES
#include <math.h>

namespace mcgbri004 {
float* getGausianMatrix(float sigma, int w);
float getGausianValue(float sigma, int x, int y);
int* applyGausianBlur(int* image, int imageXLen, int imageYLen, float sigma, int w);
}
#endif // GAUSSIANBLUR_H
