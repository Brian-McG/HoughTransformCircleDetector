// Brian Mc George
// MCGBRI004

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include "include/edgedetection.hpp"
#include "include/imagefilter.hpp"
#include "include/imageutil.hpp"

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

/**
 * @return a pointer to the magnitude image
 */
int* EdgeDetection::getMagnitudeImageRef() {
    if(magnitudeImage == nullptr) {
        determineMagnitudeImage();
    }
    return magnitudeImage;
}

/**
 * @return a pointer to directions
 */
float* EdgeDetection::getDirectionsRef() {
    if(directions == nullptr) {
        determineEdgeDetection();
    }
    return directions;
}

/**
 * @return a pointer to edge detection image
 */
int* EdgeDetection::getEdgeDetectionImageRef() {
    if(edgeDetectionImage == nullptr) {
        determineEdgeDetection();
    }
    return edgeDetectionImage;
}

/**
 * Determines the magnitude image
 */
void EdgeDetection::determineMagnitudeImage() {
    int w = 5;
    // 5x5 Sobel filter
    float xDeltaMatrix[] = {2, 1, 0, -1, -2, 3, 2, 0, -2, -3, 4, 3, 0, -3, -4, 3, 2, 0, -2, -3, 2, 1, 0, -1, -2};
    float yDeltaMatrix[] = {2, 3, 4, 3, 2, 1, 2, 3, 2, 1, 0, 0, 0, 0, 0, -1, -2, -3, -2, -1, -2, -3, -4, -3, -2};

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

/**
 * Determines the edge detection image using the canny edge detector
 */
void EdgeDetection::determineEdgeDetection() {
    int w = 5;
    // 5x5 Sobel filter
    float xDeltaMatrix[] = {2, 1, 0, -1, -2, 3, 2, 0, -2, -3, 4, 3, 0, -3, -4, 3, 2, 0, -2, -3, 2, 1, 0, -1, -2};
    float yDeltaMatrix[] = {2, 3, 4, 3, 2, 1, 2, 3, 2, 1, 0, 0, 0, 0, 0, -1, -2, -3, -2, -1, -2, -3, -4, -3, -2};

    // Apply x and y sobel filter across image
    int* xDeltaImage = applyImageFilter(image, imageXLen, imageYLen, xDeltaMatrix, w);
    int* yDeltaImage = applyImageFilter(image, imageXLen, imageYLen, yDeltaMatrix, w);

    float* magnitudes = new float[imageYLen*imageXLen];
    directions = new float[imageYLen*imageXLen];
    int* roundedDirections = new int[imageYLen*imageXLen];

    float maxMagnitude = -1.0f;

    /*
     * Determine direction of edge and round it to one of 4 directions (north-south, east-west and the two diagonals)
     */
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            magnitudes[y*imageXLen + x] = pow(xDeltaImage[y*imageXLen + x], 2) + pow(yDeltaImage[y*imageXLen + x], 2);
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
    delete[] xDeltaImage;
    delete[] yDeltaImage;

    // Normalise the magnitudes
    for (int y = 0; y < imageYLen; ++y) {
        for(int x =0; x < imageXLen; ++x) {
            magnitudes[y*imageXLen + x] /= maxMagnitude;
        }
    }

    // Apply non-maximal suppression and use a double threshold
    edgeDetectionImage = new int[imageYLen*imageXLen];
    int* edgeDetectionImageTmp = new int[imageYLen*imageXLen];
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
