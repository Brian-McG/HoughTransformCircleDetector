// Brian Mc George
// MCGBRI004
#include <cmath>
#include <utility>
#include <vector>
#include "linedrawingutil.hpp"

namespace mcgbri004 {
std::vector<std::pair<int, int>> & produceCircleCoordinatesForImage(int imageXLen, int imageYLen, int midPointX, int midPointY, int r) {
    int pk, x, y;
    pk = 3 - 2*r;
    x=0; y = r;
    std::vector<std::pair<int, int>> circleCoordinates;
    drawCircle(circleCoordinates, imageXLen, imageYLen, x,y,midPointX,midPointY);
    while(x < y) {
        if(pk <= 0) {
            pk = pk + (4*x) + 6;
            drawCircle(circleCoordinates, imageXLen, imageYLen, ++x,y,midPointX,midPointY);
        } else {
            pk = pk + (4*(x-y)) + 10;
            drawCircle(circleCoordinates, imageXLen, imageYLen, ++x,--y,midPointX,midPointY);
        }
    }

    return circleCoordinates;
}

void drawCircle(std::vector<std::pair<int, int>> &coordinates, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY) {
    addPointToCoordinates(coordinates, imageXLen, imageYLen, x + midPointX, y + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, -x + midPointX, y + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, x + midPointX, -y + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, -x + midPointX, -y + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, y + midPointX, x + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, y + midPointX, -x + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, -y + midPointX, x + midPointY);
    addPointToCoordinates(coordinates, imageXLen, imageYLen, -y + midPointX, -x + midPointY);
}

void addPointToCoordinates(std::vector<std::pair<int, int>> &coordinates, int imageXLen, int imageYLen, int x, int y) {
    if(x >= 0 && x < imageXLen && y >= 0 && y < imageYLen) {
        std::pair<int, int> coordinate(x, y);
        coordinates.push_back(coordinate);
    }
}

void addCircleToImage(int* image, int imageXLen, std::vector<std::pair<int, int>>& coordinates) {
    for(std::pair<int, int> coordinate : coordinates) {
        image[coordinate.first + imageXLen * coordinate.second] = -1;
    }
}
}
