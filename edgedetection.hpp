#ifndef EDGEDETECTION_HPP
#define EDGEDETECTION_HPP

namespace mcgbri004 {
class EdgeDetection {
private:
    int imageXLen;
    int imageYLen;
    float* directions;
    int* magnitudeImage;
    int* edgeDetectionImage;
    int* image;
    void determineEdgeDetection();
    void determineMagnitudeImage();
public:
    virtual ~EdgeDetection();
    EdgeDetection(int* image, int imageXLen, int imageYLen);
    float* getDirectionsRef();
    int* getMagnitudeImageRef();
    int* getEdgeDetectionImageRef();
};
}
#endif // EDGEDETECTION_HPP
