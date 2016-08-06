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

    int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y) {
        if(y < 0 || x < 0) {
            return 0;
        } else if (y >= imageYAxisLength || x >= imageXAxisLength) {
            return 0;
        }
        return image[y*imageXAxisLength + x];
    }
}
