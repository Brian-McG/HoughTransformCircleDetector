// Brian Mc George
// MCGBRI004
#include <cmath>
#include <iostream>

namespace mcgbri004 {
int getBoundryReflectedIndex(int imageAxisLength, int index) {
    if(index < 0) {
        index = abs(index + 1);
    } else if (index >= imageAxisLength) {
        int diff = index - imageAxisLength;
        index = index - (1 + diff);
    }
    return index;
}

float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return 0;
    }
    return image[y*imageXAxisLength + x];
}

float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z) {
    if(y < 0 || x < 0 || z < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength || z >= imageZAxisLength) {
        return 0;
    }
    return image[x + y*imageXAxisLength + z*imageXAxisLength*imageYAxisLength];
}

int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return 0;
    }
    return image[y*imageXAxisLength + x];
}

int getBoundryNegatedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
    if(y < 0 || x < 0) {
        return -1;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
        return -1;
    }
    return image[y*imageXAxisLength + x];
}

int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z) {
    if(y < 0 || x < 0 || z < 0) {
        return 0;
    } else if (y >= imageYAxisLength || x >= imageXAxisLength || z >= imageZAxisLength) {
        return 0;
    }
    return image[x + y*imageXAxisLength + z*imageXAxisLength*imageYAxisLength];
}

//void incrementWithOverlap(float* image, int imageXAxisLength, int imageYAxisLength, int overlapProportion, int x, int y) {
//    int xOverlap = imageXAxisLength / overlapProportion;
//    int yOverlap = imageYAxisLength / overlapProportion;
//    if(y < 0 || x < 0) {

//    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {

//    }
//    ++image[(y + overlapProportion)*(imageXAxisLength + 2 * xOverlap) + (x + overlapProportion)];
//}
//void incrementWithOverlap(int* image, int imageXAxisLength, int imageYAxisLength, int overlapProportion, int x, int y) {
//    int xOverlap = imageXAxisLength / overlapProportion;
//    int yOverlap = imageYAxisLength / overlapProportion;
//    if(y < 0 || x < 0) {

//    } else if (y >= imageYAxisLength || x >= imageXAxisLength) {

//    }
//    ++image[(y + overlapProportion)*((imageXAxisLength + 2 * xOverlap) + 2 * xOverlap) + (x + overlapProportion)];
//}
//void incrementWithOverlap(float* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int overlapProportion, int x, int y, int z) {
//    int xOverlap = imageXAxisLength / overlapProportion;
//    int yOverlap = imageYAxisLength / overlapProportion;
//    if(y < 0 || x < 0 || z < 0) {

//    } else if (y >= imageYAxisLength || x >= imageXAxisLength || z >= imageXAxisLength) {

//    }
//    ++image[(y + overlapProportion)*(imageXAxisLength + 2 * xOverlap) + (x + overlapProportion) + z*(imageXAxisLength + 2 * xOverlap)*(imageYAxisLength + 2 * yOverlap)];
//}
void incrementWithOverlap(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int overlapProportion, int x, int y, int z) {
    int xOverlap = imageXAxisLength / overlapProportion;
    int yOverlap = imageYAxisLength / overlapProportion;
    if(z < 0 || z >= imageZAxisLength) {
        //std::cout << "WARNING: <incrementWithOverlap> invalid z input" << std::endl;
        return;
    }
    if (y < -yOverlap || x < -xOverlap || x > imageXAxisLength + xOverlap || y > imageYAxisLength + yOverlap) {
        //std::cout << "WARNING: (incrementWithOverlap) outside overlap boundries" << std::endl;
        return;
    }
    ++image[(y + yOverlap)*(imageXAxisLength + 2 * xOverlap) + (x + xOverlap) + z*(imageXAxisLength + 2 * xOverlap)*(imageYAxisLength + 2 * yOverlap)];
}
}
