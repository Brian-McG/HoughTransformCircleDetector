// Brian Mc George
// MCGBRI004
#include <iostream>
#include <cmath>
#include <vector>
#include "edgedetection.hpp"
#include "circlehoughtransform.hpp"
#include "linedrawingutil.hpp"
#include "imageutil.hpp"

namespace mcgbri004 {
int* getCirclesInImage(EdgeDetection* edgeDetector, int imageXLen, int imageYLen) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();
    const int overlapProportion = 2;
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;
    int* circleDetectionImage = new int[imageXLen * imageYLen];
    int rStart = 10;
    int rEnd = 60;
    int* accumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rEnd - rStart)];
    for (int r = 0; r < rEnd-rStart; ++r) {
        for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
            for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
                accumulator[y*(imageXLen + (2 * xOverlap)) + x + r*(imageYLen + (2 * yOverlap))*(imageXLen + (2 * xOverlap))] = 0;
            }
        }
    }
    int count = 0;
    for (int r = rStart; r < rEnd; ++r) {
        for (int y = 0; y < imageYLen; ++y) {
            for(int x =0; x < imageXLen; ++x) {
                if(edgeDetectedImage[y*imageXLen + x] == 255) {
                    int x2 = x + (r * cos(directions[y*imageXLen + x] * M_PI / 180.0));
                    int y2 = y + (r * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                    int x3 = x + round(r * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                    int y3 = y + round(r * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                    //incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x2, y2, r-rStart);
                    incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x3, y3, r-rStart);
                }
            }
        }
    }
    float* cleanedAccumulator = new float[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rEnd - rStart)];
    int pixelWindow = 3;
    for (int r = rStart; r < rEnd; ++r) {
        for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
            for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
                bool viable = false;
                int accumulatorValue = accumulator[x + y*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
                if(accumulatorValue > 10) {
                    for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                        for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x+xDelta) - xOverlap, (y+yDelta) - yOverlap, r);
                            float evaluationScore = evaluateCandidateCircle(edgeDetector, imageXLen, imageYLen, circleCoordinates);
                            cleanedAccumulator[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = (accumulatorValue * evaluationScore * r);
                        }
                    }
                } else {
                    cleanedAccumulator[x + y*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = 0;
                }
            }
        }
    }
    int* finalAccumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap))];

    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
            finalAccumulator[y*(imageXLen + (2 * xOverlap)) + x] = 0;
        }
    }
    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
            int maxX = -1;
            int maxY = -1;
            int maxR = -1;
            float maxValue = 0;
            for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                    for (int r = rStart; r < rEnd; ++r) {
                        float localValue = getBoundryZeroedValue(cleanedAccumulator, (imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), rEnd-rStart, x+xDelta, y+yDelta, r-rStart);
                        if(localValue > maxValue && localValue > 30) {
                            maxValue = localValue;
                            maxX = x+xDelta;
                            maxY = y+yDelta;
                            maxR = r-rStart;
                        }
                    }
                }
            }
            if(maxX == x && maxY == y) {
                finalAccumulator[maxY*(imageXLen + (2 * xOverlap)) + maxX] = cleanedAccumulator[maxX + maxY*(imageXLen + (2 * xOverlap)) + maxR*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
                std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage((imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), maxX, maxY, (maxR+rStart));
                addCircleToImage(finalAccumulator, (imageXLen + (2 * xOverlap)), circleCoordinates);
            }
        }

    }
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            circleDetectionImage[x + imageXLen * y] = finalAccumulator[(x + xOverlap) + (imageXLen + (2 * xOverlap)) * (y + yOverlap)];
        }
    }

    return circleDetectionImage;
}

float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates) {
    int correctEvaluations = 0;
    int totalEvaluations = circleCoordinates.size();
    int pixelWindow = 0;
    int* edgeImage = edgeDetector->getEdgeDetectionImageRef();

    for(int i = 0; i < totalEvaluations; ++i) {
        int found = false;
        for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
            for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                int value = getBoundryZeroedValue(edgeImage, imageXLen, imageYLen, circleCoordinates[i].first + xDelta, circleCoordinates[i].second+yDelta);
                if(value == 255 && !found) {
                    ++correctEvaluations;
                    found = true;
                }
            }
        }
    }
    float evaluationScore = float(correctEvaluations) / float(totalEvaluations);
    //std::cout << correctEvaluations << " " << totalEvaluations << " " << evaluationScore << std::endl;
    return evaluationScore;
}
}
