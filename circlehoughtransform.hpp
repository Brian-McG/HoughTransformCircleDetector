#ifndef CIRCLEHOUGHTRANSFORM_HPP
#define CIRCLEHOUGHTRANSFORM_HPP
#include "edgedetection.hpp"
namespace mcgbri004 {
int* getCirclesInImage(EdgeDetection* edgeDetector, int imageXLen, int imageYLen);
float evaluateCandidateCircle(EdgeDetection* edgeDetector, int imageXLen, int imageYLen, std::vector<std::pair<int, int>>& circleCoordinates);
}
#endif // CIRCLEHOUGHTRANSFORM_HPP
