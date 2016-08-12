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
    this->xOverlap = imageXLen / overlapProportion;
    this->yOverlap = imageYLen / overlapProportion;
}

CircleHoughTransform::~CircleHoughTransform() {
    delete[] circleDetectionImage;
    delete[] accumulatorImageMatrix;
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
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;

    /*
     * Given that we know the angle of the edge, we can determine points along the normal to the curve.
     * We add the points on both sides of the edge as there a few cases where only using one will fail.
     * We pick the two points that lie r length away from the edge point along the normal.
     *
     * The accumulator has an overflow to allow the determination of partial circles on the boundries.
     */
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            if(edgeDetectedImage[y*imageXLen + x] == 255) {
                int x2 = x + (rLength * cos(directions[y*imageXLen + x] * M_PI / 180.0));
                int y2 = y + (rLength * sin(directions[y*imageXLen + x] * M_PI / 180.0));
                int x3 = x + round(rLength * cos((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                int y3 = y + round(rLength * sin((directions[y*imageXLen + x] + 180) * M_PI / 180.0));
                incrementWithOverflow(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x2, y2, rIndex);
                incrementWithOverflow(accumulator, imageXLen, imageYLen, rEnd-rStart, overlapProportion, x3, y3, rIndex);
            }
        }
    }

    /*
     * Evaluate the accumulation slide, filtering out small accumulation values.
     * For the values that pass the filter, evaluate how well a circle with the input r length matches the already defined edges.
     * The accumulator slice is transformed into a score from 0 - 100 defining how well the circle fits.
     *
     * It is important to note that accumulatorImageMatrix and filteredAccumulatorImageMatrix play no role in the algorithm and are only used for visualisation purposes.
     */
    int * evaluationMatrix = new int [(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap))];
    std::fill_n(evaluationMatrix, (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)), 0);
    for (int y = 0; y < (imageYLen + (2 * yOverlap)); ++y) {
        for(int x = 0; x < (imageXLen + (2 * xOverlap)); ++x) {
            int accumulatorValue = accumulator[x + y*(imageXLen + (2 * xOverlap)) + (rIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))];

            // Visualisation of accumulator
            accumulatorImageMatrix[x + y*(imageXLen + (2 * xOverlap)) + (rLength - rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = accumulatorValue;

            int accumulatorCriterion = 20;
            // Provide more leaniant accumulation criterion for shapes on boundries which are small
            if(rLength < 25 && (x - xOverlap < 0 || y - yOverlap < 0 || x - xOverlap > imageXLen || y - yOverlap > imageYLen)) {
                accumulatorCriterion = 5;
            } else {
                accumulatorCriterion = 10;
            }
            if(accumulatorValue > accumulatorCriterion) {
                // We evaluate neighbouring pixels too since we are using a discrete value of r so we assume there could be some inaccuracies
                for(int yDelta = -pixelWindow; yDelta <= pixelWindow; ++yDelta) {
                    for (int xDelta = -pixelWindow; xDelta <= pixelWindow; ++xDelta) {
                        if(getBoundryNegatedValue(evaluationMatrix, (imageXLen + (2 * xOverlap)), (imageYLen + (2 * yOverlap)), x+xDelta, y+yDelta) == 0) {
                            // Produce a test circle to evaluate
                            std::vector<std::pair<int, int>> circleCoordinates = produceCircleCoordinatesForImage(imageXLen, imageYLen, (x+xDelta) - xOverlap, (y+yDelta) - yOverlap, rLength);

                            int evaluationScore = evaluateCandidateCircle(edgeDetector, imageXLen, imageYLen, circleCoordinates, 0) * 100;
                            if(evaluationScore > 0) {
                                evaluationMatrix[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap))] = evaluationScore;
                            } else {
                                evaluationMatrix[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap))] = -1;
                            }

                            // Visualisation of filtered accumulator
                            filteredAccumulatorImageMatrix[x + y*(imageXLen + (2 * xOverlap)) + (rLength - rStart)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap))] = evaluationMatrix[(x+xDelta) + (y+yDelta)*(imageXLen + (2 * xOverlap))];
                        }
                    }
                }
            }
        }
    }
    std::copy(evaluationMatrix, evaluationMatrix + (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)), accumulator + (rIndex)*(imageXLen + (2 * xOverlap))*(imageYLen + (2 * yOverlap)));
    delete[] evaluationMatrix;
}

/**
 * Applies the Circle Hough Transform to determine circles within the image
 *
 * @return  Reference to image with the circles highlighted
 */
int* CircleHoughTransform::getCirclesInImage() {
    if(circleDetectionImage == nullptr) {
        const int xOverlap = imageXLen / overlapProportion;
        const int yOverlap = imageYLen / overlapProportion;

        circleDetectionImage = new int[imageXLen * imageYLen];
        std::fill_n(circleDetectionImage, imageXLen * imageYLen, 0);

        /* N.B. As per Assoc. Prof. Marais's suggestion, we construct this image array for visualization purposes only.
         * Note that the algorithm presented does not use this to determine the circles in the image.
         */
        accumulatorImageMatrix = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * totalRLength];
        filteredAccumulatorImageMatrix = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * totalRLength];

        /* Declare the actual accumulator that the algorithm uses.
         * Note the size of the accumulator actually required uses a fixed z length, essentially making it O(n^2) apposed to the
         * matrix used for visualisation only, which would be O(n^3)
         */
        int * accumulator = new int[(imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * accumulatorRBufferLen];
        std::fill_n(accumulator, (imageXLen + (2 * xOverlap)) * (imageYLen + (2 * yOverlap)) * accumulatorRBufferLen, 0);

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

            /*
             * We ensure that there are always rWindow accumulator layers above and below the current r
             */
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
 * @return xOverlap for the image
 */
int CircleHoughTransform::getXOverlap() {
    return xOverlap;
}

/**
 * @return yOverlap for the image
 */
int CircleHoughTransform::getYOverlap() {
    return yOverlap;
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
