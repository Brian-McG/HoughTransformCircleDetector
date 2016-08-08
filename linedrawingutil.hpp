#ifndef LINEDRAWINGUTIL_HPP
#define LINEDRAWINGUTIL_HPP
namespace mcgbri004 {
void addCircleToImage(int* image, int imageXLen, int imageYLen, int midPointX, int midPointY, int r);
void addPointToImage(int* image, int imageXLen, int imageYLen, int x, int y);
void drawCircle(int* image, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY);
//void addLineToImage(int* image, const float x1, const float y1, const float x2, const float y2);
}
#endif // LINEDRAWINGUTIL_HPP
