#include <cmath>
#include <iostream>
#include <stdlib.h>
#include "edgedetection.hpp"
#include "imagefilter.hpp"
#include "imageutil.hpp"

namespace mcgbri004 {
EdgeDetection::EdgeDetection(int* image, int imageXLen, int imageYLen) {
    this->image = image;
    this->imageXLen = imageXLen;
    this->imageYLen = imageYLen;
    magnitudeImage = nullptr;
    directions = nullptr;
    edgeDetectionImage = nullptr;
}

EdgeDetection::~EdgeDetection() {
    delete[] magnitudeImage;
    delete[] directions;
    delete[] edgeDetectionImage;
}

int* EdgeDetection::getMagnitudeImageRef() {
    if(magnitudeImage == nullptr) {
        determineMagnitudeImage();
    }
    return magnitudeImage;
}

float* EdgeDetection::getDirectionsRef() {
    if(directions == nullptr) {
        determineEdgeDetection();
    }
    return directions;
}

int* EdgeDetection::getEdgeDetectionImageRef() {
    if(edgeDetectionImage == nullptr) {
        determineEdgeDetection();
    }
    return edgeDetectionImage;
}

void EdgeDetection::determineMagnitudeImage() {
    // Use Intel's IPP definition of a 5x5 sobel filter (https://software.intel.com/en-us/node/504204):
    int w = 5;
    float xDeltaMatrix[] = {-1, -2, 0, -2, -1, -4, -8, 0, 8, 4, -6, -12, 0, 12, 6, -4, -8, 0, 8, 4, -1, -2, 0, 2, 1};
    float yDeltaMatrix[] = {-1, -4, -6, -4, -1, -2, -8, -12, -8, -2, 0, 0, 0, 0, 0, 2,  8, 12, 8, 2, 1, 4, 6, 4, 1};

    int* xDeltaImage = applyImageFilter(image, imageXLen, imageYLen, xDeltaMatrix, w);
    int* yDeltaImage = applyImageFilter(image, imageXLen, imageYLen, yDeltaMatrix, w);
    magnitudeImage = new int[imageYLen*imageXLen];
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            magnitudeImage[y*imageXLen + x] = round(sqrt(pow(xDeltaImage[y*imageXLen + x], 2) + pow(yDeltaImage[y*imageXLen + x], 2)));
        }
    }
    delete[] xDeltaImage;
    delete[] yDeltaImage;
}

void EdgeDetection::determineEdgeDetection() {
    // Use Intel's IPP definition of a 5x5 sobel filter (https://software.intel.com/en-us/node/504204):
    int w = 5;
//    float xDeltaMatrix[] = {-1, -2, 0, -2, -1, -4, -8, 0, 8, 4, -6, -12, 0, 12, 6, -4, -8, 0, 8, 4, -1, -2, 0, 2, 1};
//    float yDeltaMatrix[] = {-1, -4, -6, -4, -1, -2, -8, -12, -8, -2, 0, 0, 0, 0, 0, 2,  8, 12, 8, 2, 1, 4, 6, 4, 1};
    //    int w = 3;
    //    float xDeltaMatrix[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    //    float yDeltaMatrix[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    //    int w = 5;
        float xDeltaMatrix[] = {2, 1, 0, -1, -2, 3, 2, 0, -2, -3, 4, 3, 0, -3, -4, 3, 2, 0, -2, -3, 2, 1, 0, -1, -2};
        float yDeltaMatrix[] = {2, 3, 4, 3, 2, 1, 2, 3, 2, 1, 0, 0, 0, 0, 0, -1, -2, -3, -2, -1, -2, -3, -4, -3, -2};
    int* xDeltaImage = applyImageFilter(image, imageXLen, imageYLen, xDeltaMatrix, w);
    int* yDeltaImage = applyImageFilter(image, imageXLen, imageYLen, yDeltaMatrix, w);
    float* magnitudes = new float[imageYLen*imageXLen];
    std::fill(magnitudes, magnitudes + imageYLen*imageXLen, 0);
    directions = new float[imageYLen*imageXLen];
    int* roundedDirections = new int[imageYLen*imageXLen];
    float maxMagnitude = -1.0f;
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            magnitudes[y*imageXLen + x] = sqrt(pow(xDeltaImage[y*imageXLen + x], 2) + pow(yDeltaImage[y*imageXLen + x], 2));
            if (maxMagnitude < magnitudes[y*imageXLen + x]) {
                maxMagnitude = magnitudes[y*imageXLen + x];
            }

            float direction = (atan2(float(yDeltaImage[y*imageXLen + x]), float(xDeltaImage[y*imageXLen + x]))  * 180 / M_PI) + 180;
            directions[y*imageXLen + x] = direction;
            if(direction < 22.5f || (direction >= 157.5f && direction < 202.5f ) || (direction >= 337.5f && direction <= 360.0f)) {
                roundedDirections[y*imageXLen + x] = 0;
            } else if ((direction >= 22.5f && direction < 67.5f) || (direction >= 202.5f && direction < 247.5f)) {
                roundedDirections[y*imageXLen + x] = 45;
            } else if ((direction >= 67.5f && direction < 112.5f) || (direction >= 247.5f && direction < 292.5f)) {
                roundedDirections[y*imageXLen + x] = 90;
            } else if ((direction >= 112.5f && direction < 157.5f) || (direction >= 292.5f && direction < 337.5f)) {
                roundedDirections[y*imageXLen + x] = 135;
            } else {
                // Debug - Something went wrong
                std::cout << "Unaccounted direction" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    delete xDeltaImage;
    delete yDeltaImage;

    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            magnitudes[y*imageXLen + x] /= maxMagnitude;
        }
    }

    edgeDetectionImage = new int[imageYLen*imageXLen];
    std::fill(edgeDetectionImage, edgeDetectionImage + imageYLen * imageXLen, 0);
    int* edgeDetectionImageTmp = new int[imageYLen*imageXLen];
    std::fill(edgeDetectionImageTmp, edgeDetectionImageTmp + imageYLen * imageXLen, 0);
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            char isMaximum = 0;
            char maximumValue = 0;
            if(magnitudes[y*imageXLen + x] > 0.29f) {
                maximumValue = 2;
            } else if(magnitudes[y*imageXLen + x] > 0.05f) {
                maximumValue = 1;
            }
            if(maximumValue != 0) {
                if(roundedDirections[y*imageXLen + x] == 0) {
                    float mag1 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x + 1, y);
                    float mag2 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x - 1, y);
                    if(mag1 < magnitudes[y*imageXLen + x] && mag2 < magnitudes[y*imageXLen + x]) {
                        isMaximum = maximumValue;
                    }
                } else if(roundedDirections[y*imageXLen + x] == 45) {
                    float mag1 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x - 1, y - 1);
                    float mag2 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x + 1, y + 1);
                    if(mag1 < magnitudes[y*imageXLen + x] && mag2 < magnitudes[y*imageXLen + x]) {
                        isMaximum = maximumValue;
                    }
                } else if (roundedDirections[y*imageXLen + x] == 90) {
                    float mag1 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x, y + 1);
                    float mag2 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x, y - 1);
                    if(mag1 < magnitudes[y*imageXLen + x] && mag2 < magnitudes[y*imageXLen + x]) {
                        isMaximum = maximumValue;
                    }
                } else {
                    float mag1 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x + 1, y - 1);
                    float mag2 = getBoundryZeroedValue(magnitudes, imageXLen, imageYLen, x - 1, y + 1);
                    if(mag1 < magnitudes[y*imageXLen + x] && mag2 < magnitudes[y*imageXLen + x]) {
                        isMaximum = maximumValue;
                    }
                }
            }
            if(isMaximum == 2) {
                edgeDetectionImage[y*imageXLen + x] = 255;
                edgeDetectionImageTmp[y*imageXLen + x] = 255;
            } else if(isMaximum == 1) {
                // We will modify this during hysteresis
                edgeDetectionImage[y*imageXLen + x] = -1;
                edgeDetectionImageTmp[y*imageXLen + x] = 0;
            } else {
                edgeDetectionImage[y*imageXLen + x] = 0;
                edgeDetectionImageTmp[y*imageXLen + x] = 0;
            }

        }
    }

    // Apply hysteresis
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            if(edgeDetectionImage[y*imageXLen + x] == -1) {
                int boundaryValues[] = {getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x + 1, y),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x - 1, y),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x + 1, y - 1),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x - 1, y + 1),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x, y + 1),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x, y - 1),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x - 1, y - 1),
                                        getBoundryZeroedValue(edgeDetectionImageTmp, imageXLen, imageYLen, x + 1, y + 1)};
                edgeDetectionImage[y*imageXLen + x] = 0;
                for(int i = 0; i < 8; ++i) {
                    if(boundaryValues[i] == 255) {
                        edgeDetectionImage[y*imageXLen + x] = 255;
                        break;
                    }
                }
            }
        }
    }
    delete[] edgeDetectionImageTmp;
    delete[] magnitudes;
    delete[] roundedDirections;
}
}
