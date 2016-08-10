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

void fillAccumulationLayer(int * accumulator, EdgeDetection* edgeDetector, int imageXLen, int imageYLen, int rStart, int rEnd, int pixelWindow, int overlapProportion, int r) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;

    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            if(edgeDetectedImage[y*imageXLen + x] == 255) {
                int x2 = x + (r * cos(directions[y*imageXLen + x] * M_PI / 180.0));
                int y2 = y + (r * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                int x3 = x + round(r * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                int y3 = y + round(r * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x2, y2, r-rStart);
//                    incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x3, y3, r-rStart);
            }
        }
    }
    std::cout << "DEbug -1" << std::endl;
    int * evaluationMatrix = new int [(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap))];
    std::fill(evaluationMatrix, evaluationMatrix + imageXLen * imageYLen, 0);
    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x = 0; x < (imageXLen + (2 * xOverlap)); ++x) {
            int accumulatorValue = accumulator[x + y*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
            int accumulatorCriterion = 20;
            // Provide more leaniant accumulation criterion for shapes on boundries
            if(x - xOverlap < 0 || y - yOverlap < 0 || x - xOverlap > imageXLen || y - yOverlap > imageYLen) {
                accumulatorCriterion = 10;
            }
            if(accumulatorValue > accumulatorCriterion) {
                for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                    for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                        if(getBoundryNegatedValue(evaluationMatrix, imageXLen, imageYLen, x+xDelta, y+yDelta) == 0) {
                            // Produce a test circle to evaluate
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x+xDelta) - xOverlap, (y+yDelta) - yOverlap, r);

                            // We evaluate neighbouring pixels too since we are using a discrete value of r, we assume there could be some inaccuracies
                            int evaluationScore = evaluateCandidateCircle(edgeDetector, imageXLen, imageYLen, circleCoordinates) * 100;
                            if(evaluationScore > 0) {
                                evaluationMatrix[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap))] = evaluationScore;
                            } else {
                                evaluationMatrix[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap))] = -1;
                            }
                        }
                    }
                }
            }
        }
    }
    std::cout << "DEbug 0" << std::endl;
    std::copy(evaluationMatrix, evaluationMatrix + imageXLen * imageYLen, accumulator + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap)));
    std::cout << "DEbug 1" << std::endl;
}

int* getCirclesInImage(EdgeDetection* edgeDetector, int imageXLen, int imageYLen) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();
    const int overlapProportion = 4;
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;
    int* circleDetectionImage = new int[imageXLen * imageYLen];
    const int rStart = 10;
    const int rEnd = 60;
    const int rWindow = 5;
    const int pixelWindow = 3;
    int * accumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rWindow*2)];
    std::fill(accumulator, accumulator + (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rWindow*2), 0);

    // Fill r-value windowed accumulator
    for (int r = rStart; r < rStart + (2 * rWindow); ++r) {
        fillAccumulationLayer(accumulator, edgeDetector, imageXLen, imageYLen, rStart, rEnd, pixelWindow, overlapProportion, r);
    }


//    float* cleanedAccumulator = new float[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rEnd - rStart)];
//    for (int r = rStart; r < rEnd; ++r) {
//        for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
//            for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
//                int accumulatorValue = accumulator[x + y*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
//                int accumulatorCriterion = 20;
//                if(x - xOverlap < 0 || y - yOverlap < 0 || x - xOverlap > imageXLen || y - yOverlap > imageYLen) {
//                    accumulatorCriterion = 10;
//                }
//                if(accumulatorValue > accumulatorCriterion) {
//                    for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
//                        for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
//                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x+xDelta) - xOverlap, (y+yDelta) - yOverlap, r);
//                            float evaluationScore = evaluateCandidateCircle(edgeDetector, imageXLen, imageYLen, circleCoordinates);
//      std::cout << "DEbug 0" << std::endl;                      cleanedAccumulator[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = evaluationScore;
//                        }
//                    }
//                } else {
//                    cleanedAccumulator[x + y*(imageXLen + (2 * xOverlap)) + (r-rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = 0;
//                }
//            }
//        }
//    }fillAccumulationLayer(int * accumulator, int imageXLen, int imageYLen, int rLength, int overlapProportion, int r
//    int* finalAccumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap))];

//    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
//        for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
//            finalAccumulator[y*(imageXLen + (2 * xOverlap)) + x] = 0;
//        }
//    }

//    for (int r = rStart; r < rEnd; ++r) {
//        for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
//            for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
//                int maxX = -1;
//                int maxY = -1;
//                int maxR = -1;
//                float maxValue = 0;
//                for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
//                    for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
//                        for (int rDelta = -rWindow; rDelta <= rWindow; ++rDelta) {
//                            float localValue = getBoundryZeroedValue(cleanedAccumulator, (imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), rEnd-rStart, x+xDelta, y+yDelta, (r-rStart) + rDelta);
//                            float valueCriterion = 0.5f;
//                            if((x+xDelta) - xOverlap < 0 || (y+yDelta) - yOverlap < 0 || (x+xDelta) - xOverlap > imageXLen || (y+yDelta) - yOverlap > imageYLen) {
//                                valueCriterion = 0.5f;
//                            }
//                            if(localValue > maxValue && localValue > valueCriterion) {
//                                maxValue = localValue;
//                                maxX = x+xDelta;
//                                maxY = y+yDelta;
//                                maxR = r-rStart+rDelta;
//                            }
//                        }
//                    }

//                }
//                if(maxX == x && maxY == y && maxR + rStart == r) {
//                    finalAccumulator[maxY*(imageXLen + (2 * xOverlap)) + maxX] = cleanedAccumulator[maxX + maxY*(imageXLen + (2 * xOverlap)) + maxR*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
//                    std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage((imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), maxX, maxY, (maxR+rStart));
//                    addCircleToImage(finalAccumulator, (imageXLen + (2 * xOverlap)), circleCoordinates);
//                }
//            }
//        }
//    }
//    for (int y = 0; y < imageYLen; ++y) {
//        for(int x =0; x < imageXLen; ++x) {
//            circleDetectionImage[x + imageXLen * y] = finalAccumulator[(x + xOverlap) + (imageXLen + (2 * xOverlap)) * (y + yOverlap)];
//        }
//    }

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

                int value = getBoundryZeroedValue(edgeImage, imageXLen, imageYLen, circleCoordinates.at(i).first + xDelta, circleCoordinates.at(i).second+yDelta);
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
