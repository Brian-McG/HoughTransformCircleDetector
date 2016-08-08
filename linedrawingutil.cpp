// Brian Mc George
// MCGBRI004
#include <cmath>
#include <utility>
#include "linedrawingutil.hpp"

namespace mcgbri004 {
void addCircleToImage(int* image, int imageXLen, int imageYLen, int midPointX, int midPointY, int r) {
    int pk, x, y;
    pk = 3 - 2*r;
    x=0; y = r;
    drawCircle(image, imageXLen, imageYLen, x,y,midPointX,midPointY);
    while(x < y)
    {
        if(pk <= 0)
        {
            pk = pk + (4*x) + 6;
            drawCircle(image, imageXLen, imageYLen, ++x,y,midPointX,midPointY);
        }
        else
        {
            pk = pk + (4*(x-y)) + 10;
            drawCircle(image, imageXLen, imageYLen, ++x,--y,midPointX,midPointY);
        }
    }
}

void drawCircle(int* image, int imageXLen, int imageYLen, int x, int y, int midPointX, int midPointY) {
    addPointToImage(image, imageXLen, imageYLen, x + midPointX, y + midPointY);
    addPointToImage(image, imageXLen, imageYLen, -x + midPointX, y + midPointY);
    addPointToImage(image, imageXLen, imageYLen, x + midPointX, -y + midPointY);
    addPointToImage(image, imageXLen, imageYLen, -x + midPointX, -y + midPointY);
    addPointToImage(image, imageXLen, imageYLen, y + midPointX, x + midPointY);
    addPointToImage(image, imageXLen, imageYLen, y + midPointX, -x + midPointY);
    addPointToImage(image, imageXLen, imageYLen, -y + midPointX, x + midPointY);
    addPointToImage(image, imageXLen, imageYLen, -y + midPointX, -x + midPointY);
}

void addPointToImage(int* image, int imageXLen, int imageYLen, int x, int y) {
    //if(x >= 0 && x < imageXLen && y >= 0 && y < imageYLen) {
        image[x + y*imageXLen] = -1;
    //}
}
}
