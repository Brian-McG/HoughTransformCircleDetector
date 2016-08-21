// Brian Mc George
// MCGBRI004

#include <QAction>
#include <iostream>
#include <QMenuBar>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <QSignalMapper>
#include <QVBoxLayout>
#include <QSlider>
#include <string>
#include "include/window.hpp"
#include "include/gaussianblur.hpp"
#include "include/edgedetection.hpp"
#include "include/circlehoughtransform.hpp"
#include "include/imageutil.hpp"

window::window(QWidget *parent) : QMainWindow(parent), imageLabel(new QLabel), imageScrollArea(new QScrollArea), currentImageSelection(6) {
    accumulatorImages = nullptr;
    filteredAccumulatorImages = nullptr;
    setUpWidget();
    addActions();
    addMenus();
    addConnections();
    QString fileName = "./test_images/testseq100136.pgm";
    open(fileName);
    imageScrollArea->setVisible(true);
}

window::~window() {
    delete openAction;
    delete saveAction;
    delete inputImageAction;
    delete smoothedImageAction;
    delete magnitudeImageAction;
    delete edgeDetectionImageAction;
    delete accumulatorImageAction;
    delete filteredAccumulatorImageAction;
    delete circleDetectionImageAction;
    delete imageLabel;
    delete imageScrollArea;
    delete imageMenu;
    delete fileMenu;
    delete accumulatorSlider;
    delete layout;
    delete sliderLabel;
    delete imageDescriptionLabel;
    delete qLayoutWidget;
    delete[] accumulatorImages;
    delete[] filteredAccumulatorImages;
}

/**
 * Setup widgets
 */
void window::setUpWidget() {
    layout = new QVBoxLayout;

    // Set up image label
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageScrollArea->setWidget(imageLabel);

    // Set up slider
    accumulatorSlider = new QSlider(Qt::Horizontal);
    accumulatorSlider->setTickPosition(QSlider::TicksBelow);
    accumulatorSlider->setFocusPolicy(Qt::StrongFocus);
    accumulatorSlider->setTickInterval(12);
    accumulatorSlider->setSingleStep(1);
    accumulatorSlider->setMinimum(mcgbri004::CircleHoughTransform::R_START);
    accumulatorSlider->setMaximum(mcgbri004::CircleHoughTransform::R_END - 1);
    accumulatorSlider->setVisible(false);

    sliderLabel = new QLabel;
    std::string labelText = "Accumulator with radius set to " + std::to_string(mcgbri004::CircleHoughTransform::R_START);
    sliderLabel->setText(labelText.c_str());
    sliderLabel->setVisible(false);
    imageDescriptionLabel = new QLabel;
    layout->addWidget(sliderLabel);
    layout->addWidget(accumulatorSlider);
    layout->addWidget(imageDescriptionLabel);
    layout->addWidget(imageScrollArea);

    // Set layout in QWidget
    qLayoutWidget = new QWidget();
    qLayoutWidget->setLayout(layout);
    qLayoutWidget->activateWindow();

    // Set QWidget as the central layout of the main window
    setCentralWidget(qLayoutWidget);
}

/**
 * Initialise actions
 */
void window::addActions() {
    openAction = new QAction(tr("O&pen image"), this);
    openAction->setToolTip(tr("Opens an image"));
    saveAction = new QAction(tr("S&ave image"), this);
    saveAction->setToolTip(tr("Saves an image"));
    inputImageAction = new QAction(tr("I&nput Image"), this);
    inputImageAction->setToolTip(tr("Sets image to input image"));
    inputImageAction->setCheckable(true);
    smoothedImageAction = new QAction(tr("S&moothed Image"), this);
    smoothedImageAction->setToolTip(tr("Sets image to the smoothed image"));
    smoothedImageAction->setCheckable(true);
    magnitudeImageAction = new QAction(tr("M&agnitude Image"), this);
    magnitudeImageAction->setToolTip(tr("Sets image to the magnitude image"));
    magnitudeImageAction->setCheckable(true);
    edgeDetectionImageAction = new QAction(tr("E&dge Detection Image"), this);
    edgeDetectionImageAction->setToolTip(tr("Sets image to the edge detection image"));
    edgeDetectionImageAction->setCheckable(true);
    accumulatorImageAction = new QAction(tr("A&ccumulator View"), this);
    accumulatorImageAction->setToolTip(tr("Sets image to the accumulator view"));
    accumulatorImageAction->setCheckable(true);
    filteredAccumulatorImageAction = new QAction(tr("F&iltered Accumulator View"), this);
    filteredAccumulatorImageAction->setToolTip(tr("Sets image to the filtered accumulator view"));
    filteredAccumulatorImageAction->setCheckable(true);
    circleDetectionImageAction = new QAction(tr("C&ircle Detection Image"), this);
    circleDetectionImageAction->setToolTip(tr("Sets image to the circle detection image"));
    circleDetectionImageAction->setCheckable(true);
}

/**
 * Setup menu bar
 */
void window::addMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    imageMenu = menuBar()->addMenu(tr("&Pick Image"));
    imageMenu->addAction(inputImageAction);
    imageMenu->addAction(smoothedImageAction);
    imageMenu->addAction(magnitudeImageAction);
    imageMenu->addAction(edgeDetectionImageAction);
    imageMenu->addAction(accumulatorImageAction);
    imageMenu->addAction(filteredAccumulatorImageAction);
    imageMenu->addAction(circleDetectionImageAction);
}

/**
 * Connect actions to various functions
 */
void window::addConnections() {
    connect(openAction, &QAction::triggered, this, [this]{QString fileName = ""; open(fileName); });
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(inputImageAction, SIGNAL(triggered()), this, SLOT(selectInputImage()));
    connect(smoothedImageAction, SIGNAL(triggered()), this, SLOT(selectSmoothedImage()));
    connect(magnitudeImageAction, SIGNAL(triggered()), this, SLOT(selectMagnitudeImage()));
    connect(edgeDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectEdgeDetectionImage()));
    connect(accumulatorImageAction, &QAction::triggered, this, [this]{selectAccumulatorImage(accumulatorSlider->value());});
    connect(filteredAccumulatorImageAction, &QAction::triggered, this, [this]{selectFilteredAccumulatorImage(accumulatorSlider->value());});
    connect(circleDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectCircleDetectionImage()));
    connect(accumulatorSlider, SIGNAL(valueChanged(int)), this, SLOT(selectCorrectAccumulatorImage(int)));
}

/**
 * Loads image at fileName
 *
 * @param filePath  Path to image
 */
void window::open(QString & filePath) {
    // Open file dialog if no file path provided
    if(filePath == "") {
        filePath = QFileDialog::getOpenFileName(this, tr("Open image file"), "", tr("Supported Images (*.pgm *.ppm *.pbm *.jpeg *.jpg *.png)"));
    } if(filePath != "") {
        QPixmap originalPixmap(filePath);

        // Load input image
        inputImage = originalPixmap.toImage();
        width = inputImage.width();
        height = inputImage.height();
        int *matrix = new int [width*height];

        for(int j = 0; j < height; j++) {
            for(int i = 0; i < width; i++) {
                matrix[j*width+i] = qGray(inputImage.pixel(i,j));
            }
        }

        // Blur image
        int* blurredMatrix = mcgbri004::applyGaussianBlur(matrix, width, height, 1.4, 5);
        smoothedImage = QImage(width, height, QImage::Format_RGB32);
        setQImage(smoothedImage, blurredMatrix, width, height);

        // Magnitude image
        mcgbri004::EdgeDetection * edgeDetector = new mcgbri004::EdgeDetection(blurredMatrix, width, height);
        int* magnitudeMatrix = edgeDetector->getMagnitudeImageRef();
        magnitudeImage = QImage(width, height, QImage::Format_RGB32);
        setQImage(magnitudeImage, magnitudeMatrix, width, height);

        // Edge detected image
        int* edgeMatrix =  edgeDetector->getEdgeDetectionImageRef();
        edgeDetectionImage = QImage(width, height, QImage::Format_RGB32);
        setQImage(edgeDetectionImage, edgeMatrix, width, height);
        delete[] blurredMatrix;

        // circle detected image
        mcgbri004::CircleHoughTransform * circleHoughTransform = new mcgbri004::CircleHoughTransform(edgeDetector, width, height);
        int* circleDetectedMatrix = circleHoughTransform->getCirclesInImage();
        circleDetectionImage = QImage(width, height, QImage::Format_RGB32);
        for(int j = 0; j < height; j++) {
            for(int i = 0; i < width; i++) {
                if(circleDetectedMatrix[j*width+i] != 0) {
                    circleDetectionImage.setPixel(i, j, qRgb(255, 0, 0));
                } else {
                    circleDetectionImage.setPixel(i, j, qRgb(matrix[j*width+i], matrix[j*width+i], matrix[j*width+i]));
                }
            }
        }
        delete[] matrix;

        if(accumulatorImages != nullptr) {
            delete[] accumulatorImages;
        } if(filteredAccumulatorImages != nullptr) {
            delete[] filteredAccumulatorImages;
        }


        // Accumulator images
        int accumulatorXLen = circleHoughTransform->getAccumulatorXLen();
        int accumulatorYLen = circleHoughTransform->getAccumulatorYLen();
        accumulatorImages = new QImage[circleHoughTransform->TOTAL_R_LENGTH];
        int* imageMatrix = circleHoughTransform->getAccumulatorImageMatrix();
        for(int r = 0; r < circleHoughTransform->TOTAL_R_LENGTH; ++r) {
            accumulatorImages[r] = QImage(accumulatorXLen, accumulatorYLen, QImage::Format_RGB32);
            for(int j = 0; j < accumulatorYLen; j++) {
                for(int i = 0; i < accumulatorXLen; i++) {
                    if(imageMatrix[r * accumulatorXLen * accumulatorYLen + j * accumulatorXLen + i] > 0) {
                        int color = imageMatrix[r * accumulatorXLen * accumulatorYLen + j * accumulatorXLen + i] * 15 + 30;
                        if(color > 255) {
                            color = 255;
                        }
                        accumulatorImages[r].setPixel(i, j, qRgb(color, 0, 0));
                    } else {
                        accumulatorImages[r].setPixel(i, j, qRgb(0, 0, 0));
                    }
                }
            }
        }

        // Filtered accumulator images
        filteredAccumulatorImages = new QImage[circleHoughTransform->TOTAL_R_LENGTH];
        int* filteredImageMatrix = circleHoughTransform->getFilteredAccumulatorImageMatrix();
        for(int r = 0; r < circleHoughTransform->TOTAL_R_LENGTH; ++r) {
            filteredAccumulatorImages[r] = QImage(accumulatorXLen, accumulatorYLen, QImage::Format_RGB32);
            for(int j = 0; j < accumulatorYLen; j++) {
                for(int i = 0; i < accumulatorXLen; i++) {
                    if(filteredImageMatrix[r * accumulatorXLen * accumulatorYLen + j * accumulatorXLen + i] > 0) {
                        int color = imageMatrix[r * accumulatorXLen * accumulatorYLen + j * accumulatorXLen + i] * 15 + 30;
                        if(color > 255) {
                            color = 255;
                        }
                        filteredAccumulatorImages[r].setPixel(i, j, qRgb(color, 0, 0));
                    } else {
                        filteredAccumulatorImages[r].setPixel(i, j, qRgb(0, 0, 0));
                    }
                }
            }
        }
        delete edgeDetector;
        delete circleHoughTransform;

        // Present the circle detected image by default
        selectCircleDetectionImage();
    }
}

/**
 * Saves image currently presented to file
 */
void window::save() {
    QString defaultFilter = "Netpbm (*.pgm *.ppm *.pbm)";
    std::string baseName = "_image";
    std::string baseExt = ".pgm";

    if (currentImageSelection == 0) {
        baseName = "input" + baseName;
    } else if (currentImageSelection == 1) {
        baseName = "smoothed" + baseName;
    } else if (currentImageSelection == 2) {
        baseName = "magnitude" + baseName;
    } else if (currentImageSelection == 3) {
        baseName = "edge_detection" + baseName;
    } else if (currentImageSelection == 4) {
        baseName = "accumulator_slice" + baseName;
    } else if (currentImageSelection == 5) {
        baseName = "filtered_accumulator_slice" + baseName;
    } else if (currentImageSelection == 6) {
        baseName = "circle_detection" + baseName;
    } else {
        baseName = "image";
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open image file"), (baseName+baseExt).c_str(), tr("Netpbm (*.pgm *.ppm *.pbm);;PNG (*.png);;JPEG (*.jpg *.jpeg)"), &defaultFilter);
    if(fileName.toStdString() != "") {
        QString ext = QFileInfo(fileName).suffix();
        bool savedSuccessful = imageLabel->pixmap()->save(fileName, ext.toStdString().c_str());
        if(savedSuccessful == false) {
            std::cout << "ERROR: Failed to save " << fileName.toStdString() << std::endl;
        }
    }
}

void window::selectInputImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(inputImage);
    prepareWindow(modifiedPixmap, false, 0);
    inputImageAction->setChecked(true);
    imageDescriptionLabel->setText("Input Image");
}

void window::selectSmoothedImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(smoothedImage);
    prepareWindow(modifiedPixmap, false, 1);
    smoothedImageAction->setChecked(true);
    imageDescriptionLabel->setText("Gaussian Blurred Image");
}

void window::selectMagnitudeImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(magnitudeImage);
    prepareWindow(modifiedPixmap, false, 2);
    magnitudeImageAction->setChecked(true);
    imageDescriptionLabel->setText("Intensity Gradient Image using a 5x5 Sobel Filter");
}

void window::selectEdgeDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(edgeDetectionImage);
    prepareWindow(modifiedPixmap, false, 3);
    edgeDetectionImageAction->setChecked(true);
    imageDescriptionLabel->setText("Canny Edge Detected Image");
}

void window::selectAccumulatorImage(int rLength) {
    QPixmap modifiedPixmap = QPixmap::fromImage(accumulatorImages[rLength - mcgbri004::CircleHoughTransform::R_START]);
    prepareWindow(modifiedPixmap, true, 4, rLength);
    accumulatorImageAction->setChecked(true);
    imageDescriptionLabel->setText("Accumulator View");
}

void window::selectFilteredAccumulatorImage(int rLength) {
    QPixmap modifiedPixmap = QPixmap::fromImage(filteredAccumulatorImages[rLength - mcgbri004::CircleHoughTransform::R_START]);
    prepareWindow(modifiedPixmap, true, 5, rLength);
    filteredAccumulatorImageAction->setChecked(true);
    imageDescriptionLabel->setText("Filtered Accumulator View");
}

void window::selectCircleDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(circleDetectionImage);
    prepareWindow(modifiedPixmap, false, 6);
    circleDetectionImageAction->setChecked(true);
    imageDescriptionLabel->setText("Circle Hough Transform Detected Image");
}

/**
 * Determine if the regular or filtered accumulator must be shown
 *
 * @param rLength   Size of radius
 */
void window::selectCorrectAccumulatorImage(int rLength) {
    if(currentImageSelection == 4) {
        selectAccumulatorImage(rLength);
    } else if(currentImageSelection == 5) {
        selectFilteredAccumulatorImage(rLength);
    }
}

/**
 * Prepares window for showing new view
 *
 * @param pixmap            pixmap to show
 * @param isAccumulator     is the pixmap an accumulator?
 * @param currentSelection  current selected view
 */
void window::prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection) {
    prepareWindow(pixmap, isAccumulator, currentSelection, -1);
}

/**
 * Prepares window for showing new view
 *
 * @param pixmap            pixmap to show
 * @param isAccumulator     is the pixmap an accumulator?
 * @param currentSelection  current selected view
 * @param rLength           size of radius
 */
void window::prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection, int rLength) {
    if(isAccumulator) {
        std::string labelText = "Accumulator with radius set to " + std::to_string(rLength);
        sliderLabel->setText(labelText.c_str());
        resize(width + accumulatorWidthExtension, height + accumulatorHeightExtension);
    } else {
        resize(width + regularWidthExtension, height + regularHeightExtension);
    }
    currentImageSelection = currentSelection;
    setActionsVisibility(false);
    sliderLabel->setVisible(isAccumulator);
    accumulatorSlider->setVisible(isAccumulator);
    imageLabel->setPixmap(pixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
}

/**
 * Sets checkable actions to isVisible
 *
 * @param isVisible Should set actions to checked
 */
void window::setActionsVisibility(bool isVisible) {
    inputImageAction->setChecked(isVisible);
    smoothedImageAction->setChecked(isVisible);
    edgeDetectionImageAction->setChecked(isVisible);
    accumulatorImageAction->setChecked(isVisible);
    filteredAccumulatorImageAction->setChecked(isVisible);
    circleDetectionImageAction->setChecked(isVisible);
    magnitudeImageAction->setChecked(isVisible);
}

/**
 * Sets a QImage from a pointer to an int array image
 *
 * @param qImage            Reference to qImage to be set
 * @param image             Pointer to image which the qImage is set from
 * @param imageXAxisLength  Length of image along x-axis
 * @param imageYAxisLength  Length of image along y-axis
 */
void window::setQImage(QImage & qImage, int* image, int imageXAxisLength, int imageYAxisLength) {
    for(int y = 0; y < imageYAxisLength; ++y) {
        for(int x = 0; x < imageXAxisLength; ++x) {
            int value = image[y*imageXAxisLength+x];
            qImage.setPixel(x, y, qRgb(value, value, value));
        }
    }
}
