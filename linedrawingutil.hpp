#ifndef LINEDRAWINGUTIL_HPP
#define LINEDRAWINGUTIL_HPP
#include <vector>
namespace mcgbri004 {
std::vector<std::pair<int, int>> & produceCircleCoordinatesForImage(int imageXLen, int imageYLen, int midPointX, int midPointY, int r);
void addPointToCoordinates(std::vector<std::pair<int, int>>& coordinates, int imageXLen, int imageYLen, int x, int y);
void drawCircle(std::vector<std::pair<int, int>>& coordinates, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY);
void addCircleToImage(int* image, int imageXLen, std::vector<std::pair<int, int>>& coordinates);
//void addLineToImage(int* image, const float x1, const float y1, const float x2, const float y2);
}
#endif // LINEDRAWINGUTIL_HPP
