#include <cmath>
#include "edgedetection.hpp"
#include "imagefilter.hpp"

namespace mcgbri004 {

int* applyEdgeDetection(int* image, int imageXLen, int imageYLen) {
    int w = 3;
    float xDeltaMatrix[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    float yDeltaMatrix[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};


    int* xDeltaImage = applyImageFilter(image, imageXLen, imageYLen, xDeltaMatrix, w);
    int* yDeltaImage = applyImageFilter(image, imageXLen, imageYLen, yDeltaMatrix, w);
    int* magnitudeImage = new int[imageYLen*imageXLen];
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            // TODO: sqrt almost certainly not actually required
            magnitudeImage[y*imageXLen + x] = sqrt(pow(xDeltaImage[y*imageXLen + x], 2) + pow(yDeltaImage[y*imageXLen + x], 2));
        }
    }

    delete xDeltaImage;
    delete yDeltaImage;

    return magnitudeImage;
}
}
