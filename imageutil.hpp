#ifndef IMAGEUTIL_HPP
#define IMAGEUTIL_HPP
namespace mcgbri004 {
int getBoundryReflectedIndex(int imageAxisLength, int index);
float getBoundryZeroedValue(float* image, int imageXAxisLength, int imageYAxisLength, int x, int y);
int getBoundryZeroedValue(int* image, int imageXAxisLength, int imageYAxisLength, int x, int y);
}
#endif // IMAGEUTIL_HPP
