#ifndef CIRCLEHOUGHTRANSFORM_HPP
#define CIRCLEHOUGHTRANSFORM_HPP
#include <QImage>
#include "edgedetection.hpp"
namespace mcgbri004 {
float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates);
void setQImage(QImage & qImage, int* image, int rIndex, int imageXAxisLength, int imageYAxisLength);
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
    const static int overlapProportion = 4;
    const static int rStart = 10;
    const static int rEnd = 60;
    const static int totalRLength = rEnd - rStart;
    const static int rWindow = 5;
    const static int pixelWindow = 3;
    const static int accumulatorRBufferLen = (2 * rWindow) + 1;
    const int xOverlap = imageXLen / overlapProportion;
    const int yOverlap = imageYLen / overlapProportion;

    virtual ~CircleHoughTransform();
    CircleHoughTransform(EdgeDetection* edgeDetector, int imageXLen, int imageYLen);

    QImage* getAccumulatorImages();
    int* getCirclesInImage();
};
}
#endif // CIRCLEHOUGHTRANSFORM_HPP
