// Brian Mc George
// MCGBRI004
#include <cmath>
#include <utility>
#include <vector>
#include <iostream>
#include "linedrawingutil.hpp"

namespace mcgbri004 {
std::vector<std::pair<int, int>> produceCircleCoordinatesForImage(int imageXLen, int imageYLen, int midPointX, int midPointY, int r) {
    int pk, x, y;
    pk = 3 - 2*r;
    x=0; y = r;
    std::vector<std::pair<int, int>> circleCoordinates(r*r);
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
        //std::cout << x << " " << y << std::endl;
        coordinates.push_back(std::pair<int, int>(x,y));
    }
}

void addCircleToImage(int* image, int imageXLen, std::vector<std::pair<int, int>>& coordinates) {
    for(int i =0; i < coordinates.size(); ++i) {
        image[coordinates[i].first + imageXLen * coordinates[i].second] = -1;
    }
}
}
