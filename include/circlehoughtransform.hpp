// Brian Mc George
// MCGBRI004

#ifndef CIRCLEHOUGHTRANSFORM_HPP
#define CIRCLEHOUGHTRANSFORM_HPP

#include "include/edgedetection.hpp"

namespace mcgbri004 {
float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates, int pixelWindow);

class CircleHoughTransform {
private:
    EdgeDetection * edgeDetector; // Pointer to EdgeDetection object
    int* circleDetectionImage; // An Image that contains a circle encasing a circle in the input image
    int* accumulatorImageMatrix; // Only stored so that we can visualise the accumulator, it is not required for the core algorithm
    int* filteredAccumulatorImageMatrix;
    int imageXLen; // X-axis length of the image
    int imageYLen; // Y-axis length of the image
    void fillAccumulationLayer(int* accumulator, int r, int rIndex);
    int overlapAmount;
    int accumulatorXLen;
    int accumulatorYLen;

public:
    // Various constants for the algorithm
    const static int R_START = 10;
    const static int R_END = 61;
    const static int TOTAL_R_LENGTH = R_END - R_START;
    const static int R_WINDOW = 5;
    const static int PIXEL_WINDOW = 3;
    const static int ACCUMULATOR_R_BUFFER_LEN = (2 * R_WINDOW) + 1;

    virtual ~CircleHoughTransform();
    CircleHoughTransform(EdgeDetection* edgeDetector, int imageXLen, int imageYLen);

    int* getAccumulatorImageMatrix();
    int* getFilteredAccumulatorImageMatrix();
    int* getCirclesInImage();
    int getOverlapAmount();
    int getAccumulatorXLen();
    int getAccumulatorYLen();
};
}
#endif // CIRCLEHOUGHTRANSFORM_HPP
