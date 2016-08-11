#ifndef CIRCLEHOUGHTRANSFORM_HPP
#define CIRCLEHOUGHTRANSFORM_HPP
#include <QImage>
#include "edgedetection.hpp"
namespace mcgbri004 {
float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates);
class CircleHoughTransform {
private:
    EdgeDetection * edgeDetector; // Pointer to EdgeDetection object
    int* circleDetectionImage; // An Image that contains a circle encasing a circle in the input image
    QImage* accumulatorImage; // Only stored so that we can visualise the accumulator, it is not required for the core algorithm
    int imageXLen; // X-axis length of the image
    int imageYLen; // Y-axis length of the image
    void fillAccumulationLayer(int* accumulator, int r, int rIndex);
public:
    // Various constants for the algorithm
    const int overlapProportion = 4;
    const int rStart = 10;
    const int rEnd = 60;
    const int totalRLength = rEnd - rStart;
    const int rWindow = 5;
    const int pixelWindow = 3;
    const int accumulatorRBufferLen = (2 * rWindow) + 1;

    virtual ~CircleHoughTransform();
    CircleHoughTransform(EdgeDetection* edgeDetector, int imageXLen, int imageYLen);

    int* getCirclesInImage();
};
}
#endif // CIRCLEHOUGHTRANSFORM_HPP
