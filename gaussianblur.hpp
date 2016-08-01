#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

float* getGausianMatrix(float sigma, int w);
float getGausianValue(float sigma, int w, int x, int y);

#endif // GAUSSIANBLUR_H
