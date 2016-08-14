// Brian Mc George
// MCGBRI004
#include <iostream>
#include <cmath>
#include <vector>
#include "include/edgedetection.hpp"
#include "include/circlehoughtransform.hpp"
#include "include/circledrawingutil.hpp"
#include "include/imageutil.hpp"

namespace mcgbri004 {
CircleHoughTransform::CircleHoughTransform(EdgeDetection* edgeDetector, int imageXLen, int imageYLen) {
    this->edgeDetector = edgeDetector;
    this->imageXLen = imageXLen;
    this->imageYLen = imageYLen;
    this->circleDetectionImage = nullptr;
    this->accumulatorImageMatrix = nullptr;
    this->filteredAccumulatorImageMatrix = nullptr;
    this->overlapAmount = rEnd; // Size of largest circle we detect and a small amount extra
    this->accumulatorXLen = (imageXLen + (2 * overlapAmount));
    this->accumulatorYLen = (imageYLen + (2 * overlapAmount));
}

CircleHoughTransform::~CircleHoughTransform() {
    delete[] circleDetectionImage;
    delete[] accumulatorImageMatrix;
    delete[] filteredAccumulatorImageMatrix;
}

/**
 * Fills one layer of the accumulator for a given rLength at a specified index in the accumulator (which has a fixed z size)
 *
 * @param accumulator   Reference to the accumulator
 * @param rLength       Circle radius
 * @param rIndex        Radius index
 */
void CircleHoughTransform::fillAccumulationLayer(int * accumulator, int rLength, int rIndex) {
    int* edgeDetectedImage = edgeDetector->getEdgeDetectionImageRef();
    float* directions = edgeDetector->getDirectionsRef();

    /*
     * Given that we know the angle of the edge, we can determine points along the normal to the curve.
     * We add the points on both sides of the edge as there a few cases where only using one will fail.
     * We pick the two points that lie r length away from the edge point along the normal.
     *
     * The accumulator has an overflow to allow the determination of partial circles on the boundaries.
     */
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            if(edgeDetectedImage[y*imageXLen + x] == 255) {
                int x2 = x + (rLength * cos(directions[y*imageXLen + x] * M_PI / 180.0));
                int y2 = y + (rLength * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                int x3 = x + round(rLength * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                int y3 = y + round(rLength * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                incrementWithOverflow(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapAmount, x2, y2, rIndex);
                incrementWithOverflow(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapAmount, x3, y3, rIndex);
            }
        }
    }

    /*
     * Evaluate the accumulation slice, filtering out small accumulation values.
     * For the values that pass the filter, evaluate how well a circle with radius r matches the already defined edges.
     * The accumulator slice is transformed into a score from 0 - 100 defining how well the circle fits.
     *
     * It is important to note that accumulatorImageMatrix and filteredAccumulatorImageMatrix play no role in the algorithm and are only used for visualisation purposes.
     */
    int * evaluationMatrix = new int [accumulatorXLen * accumulatorYLen];
    std::fill_n(evaluationMatrix, accumulatorXLen * accumulatorYLen, 0);
    for (int y = 0; y < accumulatorYLen; ++y) {
        for(int x = 0; x < accumulatorXLen; ++x) {
            int accumulatorValue = accumulator[x + y * accumulatorXLen + (rIndex) * accumulatorXLen * accumulatorYLen];

            // Visualisation of accumulator
            accumulatorImageMatrix[x + y * accumulatorXLen + (rLength - rStart) * accumulatorXLen * accumulatorYLen] = accumulatorValue;

            int accumulatorCriterion = 20;
            // Provide more lenient accumulation criterion for shapes on boundaries which are small
            if(rLength < 25 && (x - overlapAmount < 0 || y - overlapAmount < 0 || x - overlapAmount > imageXLen || y - overlapAmount > imageYLen)) {
                accumulatorCriterion = 5;
            } else {
                accumulatorCriterion = 10;
            }
            if(accumulatorValue > accumulatorCriterion) {
                // We evaluate neighbouring pixels too since we are using a discrete value of r so we assume there could be some inaccuracies
                for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                    for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                        if(getBoundaryNegatedValue(evaluationMatrix, accumulatorXLen, accumulatorYLen, x+xDelta, y+yDelta) == 0) {
                            // Produce a test circle to evaluate
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x + xDelta) - overlapAmount, (y + yDelta) - overlapAmount, rLength);

                            int evaluationScore = evaluateCandidateCircle(edgeDetector, imageXLen, imageYLen, circleCoordinates, 0) * 100;
                            if(evaluationScore > 0) {
                                evaluationMatrix[(x + xDelta) + (y + yDelta) * accumulatorXLen] = evaluationScore;
                            } else {
                                evaluationMatrix[(x + xDelta) + (y + yDelta) * accumulatorXLen] = -1;
                            }

                            // Visualisation of filtered accumulator
                            filteredAccumulatorImageMatrix[x + y * accumulatorXLen + (rLength - rStart) * accumulatorXLen * accumulatorYLen] = evaluationMatrix[(x+xDelta) + (y+yDelta) * accumulatorXLen];
                        }
                    }
                }
            }
        }
    }
    std::copy(evaluationMatrix, evaluationMatrix + accumulatorXLen * accumulatorYLen, accumulator + (rIndex) * accumulatorXLen * accumulatorYLen);
    delete[] evaluationMatrix;
}

/**
 * Applies the Circle Hough Transform to determine circles within the image
 *
 * @return  Reference to image with the circles highlighted
 */
int* CircleHoughTransform::getCirclesInImage() {
    if(circleDetectionImage == nullptr) {
        circleDetectionImage = new int[imageXLen * imageYLen];
        std::fill_n(circleDetectionImage, imageXLen * imageYLen, 0);

        /* N.B. As per Assoc. Prof. Marais's suggestion, we construct this image array for visualization purposes only.
         * Note that the algorithm presented does not use this to determine the circles in the image.
         */
        accumulatorImageMatrix = new int[accumulatorXLen * accumulatorYLen * totalRLength];
        filteredAccumulatorImageMatrix = new int[accumulatorXLen * accumulatorYLen * totalRLength];

        /* Declare the actual accumulator that the algorithm uses.
         * Note the size of the accumulator actually required uses a fixed z length, essentially making it O(n^2) apposed to the
         * matrix used for visualisation only, which would be O(n^3)
         */
        int * accumulator = new int[accumulatorXLen * accumulatorYLen * accumulatorRBufferLen];
        std::fill_n(accumulator, accumulatorXLen * accumulatorYLen * accumulatorRBufferLen, 0);

        // Fill the windowed accumulator
        for (int r = rStart; r < rStart + accumulatorRBufferLen; ++r) {
            fillAccumulationLayer(accumulator, r, r-rStart);
        }

        /*
         * Iterate through all the r values we want to try.
         * If a point has an evaluation score above a set threshold
         * and is the best in a set pixel window surrounding the current pixel
         * and is best apposed to the other r values in the windowed accumulator.
         * If so, we draw a circle at the point with the set r length.
         */
        for (int r = rStart; r < rEnd; ++r) {
            const int windowRIndex = (r - rStart) % accumulatorRBufferLen;
            for (int y = 0; y < accumulatorYLen; ++y) {
                for(int x =0; x < accumulatorXLen; ++x) {
                    int maxX = -1;
                    int maxY = -1;
                    int maxR = -1;
                    int maxValue = 0;
                    if(accumulator[x + y * accumulatorXLen + (windowRIndex) * accumulatorXLen * accumulatorYLen] > 0) {
                        for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                            for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                                for (int rDelta = -rWindow; rDelta <= rWindow; ++rDelta) {
                                    if(r - rStart + rDelta >= 0 && r + rDelta < rEnd) {
                                        const int windowRDeltaIndex = (r - rStart + rDelta) % accumulatorRBufferLen;
                                        int localValue = getBoundaryZeroedValue(accumulator, accumulatorXLen, accumulatorYLen, accumulatorRBufferLen, x+xDelta, y+yDelta, windowRDeltaIndex);
                                        int valueCriterion = 49;
                                        if((x+xDelta) - overlapAmount < 0 || (y+yDelta) - overlapAmount < 0 || (x+xDelta) - overlapAmount > imageXLen || (y+yDelta) - overlapAmount > imageYLen) {
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
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, maxX - overlapAmount, maxY - overlapAmount, maxR);
                            addCircleToImage(circleDetectionImage, imageXLen, circleCoordinates);
                        }
                    }
                }
            }

            /*
             * We ensure that there are always rWindow accumulator layers above and below the current r
             */
            if (r + 1 + rWindow < rEnd && r+1 > rWindow) {
                for (int y = 0; y < accumulatorYLen; ++y) {
                    for(int x =0; x < accumulatorXLen; ++x) {
                        accumulator[x + y * accumulatorXLen + ((r - rStart + 1 + rWindow) % accumulatorRBufferLen) * accumulatorXLen * accumulatorYLen] = 0;
                    }
                }
                fillAccumulationLayer(accumulator, r + 1 + rWindow, (r - rStart + 1 + rWindow) % accumulatorRBufferLen);
            }
        }
        delete[] accumulator;
    }
    return circleDetectionImage;
}

/**
 * @return a pointer to accumulator image
 */
int* CircleHoughTransform::getAccumulatorImageMatrix() {
    if(accumulatorImageMatrix == nullptr) {
        getCirclesInImage();
    }
    return accumulatorImageMatrix;
}

/**
 * @return a pointer to filtered accumulator image
 */
int* CircleHoughTransform::getFilteredAccumulatorImageMatrix() {
    if(filteredAccumulatorImageMatrix == nullptr) {
        getCirclesInImage();
    }
    return filteredAccumulatorImageMatrix;
}

/**
 * @return amount of overlap for the image
 */
int CircleHoughTransform::getOverlapAmount() {
    return overlapAmount;
}

/**
 * @return length of accumulator in x-axis
 */
int CircleHoughTransform::getAccumulatorXLen() {
    return accumulatorXLen;
}

/**
 * @return length of accumulator in y-axis
 */
int CircleHoughTransform::getAccumulatorYLen() {
    return accumulatorYLen;
}

/**
 * Evaluates candidate circle coordinates onto the edge matrix
 *
 * @param edgeDetector      Reference to an edge detector object
 * @param imageXLen         Length of x-axis image
 * @param imageYLen         Length of y-axis image
 * @param circleCoordinates Circle coordinates
 * @return a float between 0 to 1 where 1 is a perfect match and 0 is no match
 */
float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates, int pixelWindow) {
    int correctEvaluations = 0;
    int totalEvaluations = circleCoordinates.size();
    int* edgeImage = edgeDetector->getEdgeDetectionImageRef();

    for(int i = 0; i < totalEvaluations; ++i) {
        int found = false;
        for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
            for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {

                int value = getBoundaryZeroedValue(edgeImage, imageXLen, imageYLen, circleCoordinates.at(i).first + xDelta, circleCoordinates.at(i).second+yDelta);
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
