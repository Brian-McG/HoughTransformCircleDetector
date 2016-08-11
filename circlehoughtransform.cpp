// Brian Mc George
// MCGBRI004
#include <iostream>
#include <cmath>
#include <vector>
#include <QImage>
#include "edgedetection.hpp"
#include "circlehoughtransform.hpp"
#include "linedrawingutil.hpp"
#include "imageutil.hpp"

namespace mcgbri004 {
CircleHoughTransform::CircleHoughTransform(EdgeDetection* edgeDetector, int imageXLen, int imageYLen) {
    this->edgeDetector = edgeDetector;
    this->imageXLen = imageXLen;
    this->imageYLen = imageYLen;
    this->circleDetectionImage = nullptr;
    this->accumulatorImage = nullptr;
}

CircleHoughTransform::~CircleHoughTransform() {
    delete[] circleDetectionImage;
    delete[] accumulatorImage;
}

void CircleHoughTransform::fillAccumulationLayer(int * accumulator, int rLength, int rIndex) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;

    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            if(edgeDetectedImage[y*imageXLen + x] == 255) {
                int x2 = x + (rLength * cos(directions[y*imageXLen + x] * M_PI / 180.0));
                int y2 = y + (rLength * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                int x3 = x + round(rLength * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                int y3 = y + round(rLength * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x2, y2, rIndex);
                //                    incrementWithOverlap(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x3, y3, r-rStart);
            }
        }
    }
    int * evaluationMatrix = new int [(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap))];
    std::fill(evaluationMatrix, evaluationMatrix + (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)), 0);
    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x = 0; x < (imageXLen + (2 * xOverlap)); ++x) {
            int accumulatorValue = accumulator[x + y*(imageXLen + (2 * xOverlap)) + (rIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];
            int accumulatorCriterion = 20;
            // Provide more leaniant accumulation criterion for shapes on boundries
            if(rLength < 25 && (x - xOverlap < 0 || y - yOverlap < 0 || x - xOverlap > imageXLen || y - yOverlap > imageYLen)) {
                accumulatorCriterion = 5;
            } else {
                accumulatorCriterion = 10;
            }
            if(accumulatorValue > accumulatorCriterion) {
                for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                    for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                        if(getBoundryNegatedValue(evaluationMatrix, (imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), x+xDelta, y+yDelta) == 0) {
                            // Produce a test circle to evaluate
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x+xDelta) - xOverlap, (y+yDelta) - yOverlap, rLength);

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
    //std::copy(evaluationMatrix, evaluationMatrix + (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)), accumulator + (rIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap)));
    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x = 0; x < (imageXLen + (2 * xOverlap)); ++x) {
            accumulator[x + y*(imageXLen + (2 * xOverlap)) + (rIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = evaluationMatrix[x + y*(imageXLen + (2 * xOverlap))];
        }
    }
    delete[] evaluationMatrix;
}

int* CircleHoughTransform::getCirclesInImage() {
    if(circleDetectionImage == nullptr) {
        const int xOverlap = imageXLen / overlapProportion;
        const int yOverlap = imageYLen / overlapProportion;

        this->circleDetectionImage = new int[imageXLen * imageYLen];
        std::fill(circleDetectionImage, circleDetectionImage + imageXLen * imageYLen, 0);

        /* N.B. As per Assoc. Prof. Marais's suggestion, we construct this image array for visualization purposes only.
         * Note that the algorithm presented does not use this to determine the circles in the image.
         */
        this->accumulatorImage = new QImage[this->totalRLength];

        /* Declare the actual accumulator that the algorithm uses.
         * Note the size of the accumulator actually required uses a fixed z length, essentially making it O(n^2) apposed to the
         * matrix used for visualisation only, which would be O(n^3)
         */
        int * accumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * accumulatorRBufferLen];
        std::fill(accumulator, accumulator + (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * (rWindow*2), 0);

        // Fill r-value windowed accumulator
        for (int r = rStart; r < rStart + accumulatorRBufferLen; ++r) {
            fillAccumulationLayer(accumulator, r, r-rStart);
        }

        for (int r = rStart; r < rEnd; ++r) {
            const int windowRIndex = (r - rStart) % accumulatorRBufferLen;
            for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
                for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
                    int maxX = -1;
                    int maxY = -1;
                    int maxR = -1;
                    int maxValue = 0;
                    if(accumulator[x + y * (imageXLen + (2 * xOverlap)) + (windowRIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] > 0) {
                        for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                            for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                                for (int rDelta = -rWindow; rDelta <= rWindow; ++rDelta) {
                                    if(r - rStart + rDelta >= 0 && r + rDelta < rEnd) {
                                        const int windowRDeltaIndex = (r - rStart + rDelta) % accumulatorRBufferLen;
                                        int localValue = getBoundryZeroedValue(accumulator, (imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), accumulatorRBufferLen, x+xDelta, y+yDelta, windowRDeltaIndex);
                                        int valueCriterion = 49;
                                        if((x+xDelta) - xOverlap < 0 || (y+yDelta) - yOverlap < 0 || (x+xDelta) - xOverlap > imageXLen || (y+yDelta) - yOverlap > imageYLen) {
                                            valueCriterion = 60;
                                        }
                                        if(localValue > maxValue && localValue > valueCriterion) {
                                            maxValue = localValue;
                                            maxX = x+xDelta;
                                            maxY = y+yDelta;
                                            maxR = r + rDelta;
                                        }
                                    }
                                }
                            }
                        }
                        if(maxX == x && maxY == y && r == maxR) {
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, maxX - xOverlap, maxY - yOverlap, maxR);
                            addCircleToImage(circleDetectionImage, imageXLen, circleCoordinates);
                        }
                    }
                }
            }

            if (r + 1 + rWindow < rEnd && r+1 > rWindow) {
                for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
                    for(int x =0; x < (imageXLen + (2 * xOverlap)); ++x) {
                        accumulator[x + y*(imageXLen + (2 * xOverlap)) + ((r - rStart + 1 + rWindow) % accumulatorRBufferLen)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = 0;
                    }
                }
                fillAccumulationLayer(accumulator, r + 1 + rWindow, (r - rStart + 1 + rWindow) % accumulatorRBufferLen);
            }
        }
        delete[] accumulator;
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

                int value = getBoundryZeroedValue(edgeImage, imageXLen, imageYLen, circleCoordinates.at(i).first + xDelta, circleCoordinates.at(i).second+yDelta);
                if(value == 255 && !found) {
                    ++correctEvaluations;
                    found = true;
                }
            }
        }
    }
    float evaluationScore = float(correctEvaluations) / float(totalEvaluations);
    return evaluationScore;
}
}
