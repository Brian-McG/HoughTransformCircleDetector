// Brian Mc George
// MCGBRI004

#include <cmath>
#include "imageutil.hpp"

namespace mcgbri004 {
int* applyImageFilter(int* image, int imageXLen, int imageYLen, float* filter, int w) {
    int midpoint = w / 2;

    //TODO: Add error condition for if imageXLen < w or imageYLen < w

    int* filteredImage = new int[imageYLen * imageXLen];
    for (int y = 0; y < imageYLen; ++y) {
        for(int x = 0; x < imageXLen; ++x ) {
            float pixelValue = 0;
            for (int matrixY = 0; matrixY < w; ++matrixY) {
                for(int matrixX = 0; matrixX < w; ++matrixX) {
                    int filterXValue = matrixX - midpoint;
                    int filterYValue = matrixY - midpoint;

                    int indexX = x + filterXValue;
                    int indexY = y + filterYValue;

                    // Reflect the boundaries
                    indexX = mcgbri004::getBoundryReflectedIndex(imageXLen, indexX);
                    indexY = mcgbri004::getBoundryReflectedIndex(imageYLen, indexY);

                    pixelValue += image[(indexY*imageXLen) + indexX] * filter[(matrixY*w) + matrixX];
                }
            }
            filteredImage[y*imageXLen + x] = round(pixelValue);
        }
    }

    return filteredImage;
}
}
