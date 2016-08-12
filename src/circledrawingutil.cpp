// Brian Mc George
// MCGBRI004

#include <cmath>
#include <utility>
#include <vector>
#include <iostream>
#include "include/circledrawingutil.hpp"

namespace mcgbri004 {
/**
 * Apply an implementation of the Bresenham Circle Algorithm to produce coordinates for a set of midpoints.
 * The implementation will cull points which will fall outside the image bounds.
 *
 * @param imageXLen Length of image along x-axis
 * @param imageYLen Length of image along y-axis
 * @param midPointX x-value of midpoint of circle
 * @param midPointY y-value of midpoint of circle
 * @param r         radius of circle
 * @return vector of coordinates
 */
std::vector<std::pair<int, int>> produceCircleCoordinatesForImage(int imageXLen, int imageYLen, int midPointX, int midPointY, int r) {
    int pk, x, y;
    pk = 3 - 2*r;
    x=0; y = r;
    std::vector<std::pair<int, int>> circleCoordinates;
    circleCoordinates.reserve(r*r);
    addPointsOfCircle(circleCoordinates, imageXLen, imageYLen, x,y,midPointX,midPointY);
    while(x < y) {
        if(pk <= 0) {
            pk = pk + (4*x) + 6;
            addPointsOfCircle(circleCoordinates, imageXLen, imageYLen, ++x,y,midPointX,midPointY);
        } else {
            pk = pk + (4*(x-y)) + 10;
            addPointsOfCircle(circleCoordinates, imageXLen, imageYLen, ++x,--y,midPointX,midPointY);
        }
    }

    return circleCoordinates;
}


/**
 * Attempts to add the set of 8 points (based on x, y, x midpoint and y midpoint) to the coordinates list
 *
 * @param coordinates       Vector of coordinates to add the points to
 * @param imageXLen         Length of image along x-axis
 * @param imageYLen         Length of image along y-axis
 * @param x                 x value
 * @param y                 y value
 * @param midPointX         x-value of midpoint of circle
 * @param midPointY         y-value of midpoint of circle
 */
void addPointsOfCircle(std::vector<std::pair<int, int>> &coordinates, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY) {
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, x + midPointX, y + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, -x + midPointX, y + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, x + midPointX, -y + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, -x + midPointX, -y + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, y + midPointX, x + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, y + midPointX, -x + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, -y + midPointX, x + midPointY);
    addPointToCoordinateList(coordinates, imageXLen, imageYLen, -y + midPointX, -x + midPointY);
}


/**
 * Adds a point to the coordinates list if it is within the bounds of the image
 *
 * @param coordinates   Vector of coordinates to add the points to
 * @param imageXLen     Length of image along x-axis
 * @param imageYLen     Length of image along y-axis
 * @param x             x value
 * @param y             y value
 */
void addPointToCoordinateList(std::vector<std::pair<int, int>> &coordinates, int imageXLen, int imageYLen, int x, int y) {
    if(x >= 0 && x < imageXLen && y >= 0 && y < imageYLen) {
        coordinates.push_back(std::pair<int, int>(x,y));
    }
}


/**
 * Takes a coordinates list as input and applies the points to the input image
 *
 * @param image         Pointer to image
 * @param imageXLen     Length of image along x-axis
 * @param coordinates   Vector of coordinates
 */
void addCircleToImage(int* image, int imageXLen, std::vector<std::pair<int, int>>& coordinates) {
    for(unsigned int i =0; i < coordinates.size(); ++i) {
        image[coordinates[i].first + imageXLen * coordinates[i].second] = -1;
    }
}
}
