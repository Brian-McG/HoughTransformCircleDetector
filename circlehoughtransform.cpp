// Brian Mc George
// MCGBRI004
#include <iostream>
#include <cmath>
#include "edgedetection.hpp"
#include "circlehoughtransform.hpp"
#include "imageutil.hpp"

namespace mcgbri004 {
int* getCirclesInImage(EdgeDetection* edgeDetector, int imageXLen, int imageYLen) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();
    int* circleDetectionImage = new int[imageXLen * imageYLen];
    int rStart = 10;
    int rEnd = 60;
    int* accumulator = new int[imageXLen * imageYLen * (rEnd - rStart)];
    int count = 0;
    for (int r = rStart; r < rEnd; ++r) {
        for (int y = 0; y < imageYLen; ++y) {
            for(int x =0; x < imageXLen; ++x) {
                if(edgeDetectedImage[y*imageXLen + x] == 255) {
    //                int x2 = x + (r * cos(directions[y*imageXLen + x] * M_PI / 180.0));
    //                int y2 = y + (r * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                    int x3 = x + round(r * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                    int y3 = y + round(r * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                    /*if(x2 >= 0 && x2 < imageXLen && y2 >= 0 && y2 < imageYLen) {
                        circleDetectionImage[y2*imageXLen + x2] = -1;
                    }*/ if(x3 >= 0 && x3 < imageXLen && y3 >= 0 && y3 < imageYLen) {
                        accumulator[x3 + y3*imageXLen + (r-rStart)*imageXLen*imageYLen] += 1;
                    }
                }
            }
        }
    }
    int* cleanedAccumulator = new int[imageXLen * imageYLen * (rEnd - rStart)];
    for (int r = rStart; r < rEnd; ++r) {
        for (int y = 0; y < imageYLen; ++y) {
            for(int x =0; x < imageXLen; ++x) {
                bool localMaximum = true;
                int accumulatorValue = accumulator[x + y*imageXLen + (r-rStart)*imageXLen*imageYLen];
                if(accumulatorValue > 20) {
                    for(int yDelta = -3; yDelta <= 3; ++yDelta) {
                        for (int xDelta = -3; xDelta <= 3; ++xDelta) {
                            if((yDelta != 0 || xDelta != 0)) {
                                int localValue = getBoundryZeroedValue(accumulator, imageXLen, imageYLen, rEnd-rStart, x+xDelta, y+yDelta, r-rStart);
                                if(localValue > accumulatorValue) {
                                    localMaximum = false;
                                }
                            }
                        }
                    }
                } else {
                    localMaximum = false;
                }
                if(!localMaximum) {
                    cleanedAccumulator[x + y*imageXLen + (r-rStart)*imageXLen*imageYLen] = 0;
                } else {
                    cleanedAccumulator[x + y*imageXLen + (r-rStart)*imageXLen*imageYLen] = accumulatorValue;
                }
            }
        }
    }
    int* finalAccumulator = new int[imageXLen * imageYLen];
    int pixelWindow = 16;
    for (int y = pixelWindow; y < imageYLen; y+=pixelWindow) {
        for(int x = pixelWindow; x < imageXLen; x+=pixelWindow) {
            int maxX = -1;
            int maxY = -1;
            int maxR = -1;
            int maxValue = 0;
            for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                    if(x+xDelta < imageXLen && y+yDelta < imageYLen) {
                        finalAccumulator[(y+yDelta)*imageXLen + (x+xDelta)] = 0;
                    }
                    for (int r = rStart; r < rEnd; ++r) {
                        int localValue = getBoundryZeroedValue(cleanedAccumulator, imageXLen, imageYLen, rEnd-rStart, x+xDelta, y+yDelta, r-rStart);
                        if(localValue > maxValue) {
                            maxX = x+xDelta;
                            maxY = y+yDelta;
                            maxR = r-rStart;
                        }
                    }
                }
            }
            if(maxX != -1) {
                finalAccumulator[maxY*imageXLen + maxX] = cleanedAccumulator[maxX + maxY*imageXLen + maxR*imageXLen*imageYLen];
            }
        }

    }
    return finalAccumulator;
}
}
