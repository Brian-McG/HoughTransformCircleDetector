// Brian Mc George
// MCGBRI004
#include <cmath>
#include <iostream>
#include <QImage>

namespace mcgbri004 {
/**
 * Gets the index if it is beyond the length of the image along the axis
 *
 * @param imageAxisLength   Length of image along the axis
 * @param index             Index along axis
 * @return                  Reflected index about the axis
 */
int getBoundryReflectedIndex(int imageAxisLength, int index) {
    if(index < 0) {
        index = abs(index + 1);
    } else if (index >= imageAxisLength) {
        int diff = index - imageAxisLength;
        index = index - (1 + diff);
    }
    return index;
}

/**
 * Gets the value at specified indicies
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along y-axis
 * @param x                 x index
 * @param y                 y index
 * @return value at specified indicies if within axis lengths, else returns 0
 */
float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return 0;
    }
    return image[y*imageXAxisLength + x];
}

/**
 * Gets the value at specified indicies
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along y-axis
 * @param imageZAxisLength  length of image along z-axis
 * @param x                 x index
 * @param y                 y index
 * @param z                 z index
 * @return value at specified indicies if within axis lengths, else returns 0
 */
float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z) {
    if(y < 0 || x < 0 || z < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength || z >= imageZAxisLength) {
        return 0;
    }
    return image[x + y*imageXAxisLength + z*imageXAxisLength*imageYAxisLength];
}

/**
 * Gets the value at specified indicies
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along y-axis
 * @param x                 x index
 * @param y                 y index
 * @return value at specified indicies if within axis lengths, else returns 0
 */
int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return 0;
    }
    return image[y*imageXAxisLength + x];
}

/**
 * Gets the value at specified indicies
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along y-axis
 * @param imageZAxisLength  length of image along z-axis
 * @param x                 x index
 * @param y                 y index
 * @param z                 z index
 * @return value at specified indicies if within axis lengths, else returns 0
 */
int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z) {
    if(y < 0 || x < 0 || z < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength || z >= imageZAxisLength) {
        return 0;
    }
    return image[x + y*imageXAxisLength + z*imageXAxisLength*imageYAxisLength];
}

/**
 * Gets the value at specified indicies
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along x-axis
 * @param x                 x index
 * @param y                 y index
 * @return value at specified indicies if within axis lengths, else returns -1
 */
int getBoundryNegatedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return -1;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return -1;
    }
    return image[y*imageXAxisLength + x];
}

/**
 * Increments value at specified indicies and supports overflow of image
 *
 * @param image             Pointer to image
 * @param imageXAxisLength  length of image along x-axis
 * @param imageYAxisLength  length of image along y-axis
 * @param imageZAxisLength  length of image along z-axis
 * @param overlapProportion proportion of overlap for each axis
 * @param x                 x index
 * @param y                 y index
 * @param z                 z index
 */
void incrementWithOverflow(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int overlapProportion, int x, int y, int z) {
    int xOverlap = imageXAxisLength / overlapProportion;
    int yOverlap = imageYAxisLength / overlapProportion;
    if(z < 0 || z >= imageZAxisLength) {
        std::cout << "WARNING: <incrementWithOverlap> invalid z input" << std::endl;
        return;
    }
    if (y < -yOverlap || x < -xOverlap || x > imageXAxisLength + xOverlap || y > imageYAxisLength + yOverlap) {
        std::cout << "WARNING: (incrementWithOverlap) outside overlap boundries" << std::endl;
        return;
    }
    ++image[(y + yOverlap)*(imageXAxisLength + 2 * xOverlap) + (x + xOverlap) + z*(imageXAxisLength + 2 * xOverlap)*(imageYAxisLength + 2 * yOverlap)];
}
}
