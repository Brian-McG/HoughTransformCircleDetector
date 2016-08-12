// Brian Mc George
// MCGBRI004

#ifndef IMAGEUTIL_HPP
#define IMAGEUTIL_HPP

#include <QImage>

namespace mcgbri004 {
int getBoundryReflectedIndex(int imageAxisLength, int index);
float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int x, int y);
float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z);
int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y);
int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int x, int y, int z);
int getBoundryNegatedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y);
void incrementWithOverflow(float* image, int imageXAxisLength, int imageYAxisLength, int overlapProportion, int x, int y);
void incrementWithOverflow(int* image, int imageXAxisLength, int imageYAxisLength, int overlapProportion, int x, int y);
void incrementWithOverflow(float* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int overlapProportion, int x, int y, int z);
void incrementWithOverflow(int* image, int imageXAxisLength, int imageYAxisLength, int imageZAxisLength, int overlapProportion, int x, int y, int z);
}
#endif // IMAGEUTIL_HPP
