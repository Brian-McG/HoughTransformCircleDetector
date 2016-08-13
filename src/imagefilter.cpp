// Brian Mc George
// MCGBRI004

#include <cmath>
#include "include/imageutil.hpp"

namespace mcgbri004 {
/**
 * Convolves the image with the input filter
 *
 * @param image     Pointer to image which filter is applied to
 * @param imageXLen Length of image along x-axis
 * @param imageYLen Length of image along y-axis
 * @param filter    Pointer to filter
 * @param w         Size of filter
 * @return          Pointer to image which has had the filter applied to it
 */
int* applyImageFilter(int* image, int imageXLen, int imageYLen, float* filter, int w) {
    int midpoint = w / 2;

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
                    indexX = mcgbri004::getBoundaryReflectedIndex(imageXLen, indexX);
                    indexY = mcgbri004::getBoundaryReflectedIndex(imageYLen, indexY);

                    pixelValue += image[(indexY*imageXLen) + indexX] * filter[(matrixY*w) + matrixX];
                }
            }
            filteredImage[y*imageXLen + x] = round(pixelValue);
        }
    }

    return filteredImage;
}
}
