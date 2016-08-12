// Brian Mc George
// MCGBRI004

#ifndef LINEDRAWINGUTIL_HPP
#define LINEDRAWINGUTIL_HPP

#include <vector>

namespace mcgbri004 {
std::vector<std::pair<int, int>> produceCircleCoordinatesForImage(int imageXLen, int imageYLen, int midPointX, int midPointY, int r);
void addPointToCoordinateList(std::vector<std::pair<int, int>>& coordinates, int imageXLen, int imageYLen, int x, int y);
void addPointsOfCircle(std::vector<std::pair<int, int>>& coordinates, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY);
void addCircleToImage(int* image, int imageXLen, std::vector<std::pair<int, int>>& coordinates);
}
#endif // LINEDRAWINGUTIL_HPP
