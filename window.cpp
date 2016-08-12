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
#include "window.hpp"
#include "gaussianblur.hpp"
#include "edgedetection.hpp"
#include "circlehoughtransform.hpp"
#include "imageutil.hpp"

window::window(QWidget *parent) : QMainWindow(parent), imageLabel(new QLabel), imageScrollArea(new QScrollArea), currentImageSelection(6) {
    setUpWidget();
    addActions();
    addMenus();
    addConections();
    QString fileName = "./images/testseq100136.pgm";
    open(fileName);
    imageScrollArea->setVisible(true);
}

window::~window() { 
    delete openAction;
    delete saveAction;
    delete inputImageAction;
    delete smoothedImageAction;
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
    delete qLayoutWidget;
    delete[] accumulatorImages;
    delete[] filteredAccumulatorImages;
}

void window::setUpWidget() {
    layout = new QVBoxLayout;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageScrollArea->setWidget(imageLabel);

    accumulatorSlider = new QSlider(Qt::Horizontal);
    accumulatorSlider->setTickPosition(QSlider::TicksBelow);
    accumulatorSlider->setFocusPolicy(Qt::StrongFocus);
    accumulatorSlider->setTickInterval(12);
    accumulatorSlider->setSingleStep(1);
    accumulatorSlider->setMinimum(mcgbri004::CircleHoughTransform::rStart);
    accumulatorSlider->setMaximum(mcgbri004::CircleHoughTransform::rEnd - 1);
    accumulatorSlider->setVisible(false);

    sliderLabel = new QLabel;
    std::string labelText = "Accumulator with radius set to " + std::to_string(mcgbri004::CircleHoughTransform::rStart);
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

    // Set QWidget as the central layout of the main window
    setCentralWidget(qLayoutWidget);
}

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

void window::addConections() {
    connect(openAction, &QAction::triggered, this, [this]{QString fileName = ""; open(fileName); });
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(inputImageAction, SIGNAL(triggered()), this, SLOT(selectInputImage()));
    connect(smoothedImageAction, SIGNAL(triggered()), this, SLOT(selectSmoothedImage()));
    connect(magnitudeImageAction, SIGNAL(triggered()), this, SLOT(selectMagnitudeImage()));
    connect(edgeDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectEdgeDetectionImage()));
    connect(accumulatorImageAction, &QAction::triggered, this, [this]{selectAccumulatorImage(mcgbri004::CircleHoughTransform::rStart);});
    connect(filteredAccumulatorImageAction, &QAction::triggered, this, [this]{selectFilteredAccumulatorImage(mcgbri004::CircleHoughTransform::rStart);});
    connect(circleDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectCircleDetectionImage()));
    connect(accumulatorSlider, SIGNAL(valueChanged(int)), this, SLOT(selectCorrectAccumulatorImage(int)));
}

void window::open(QString & fileName) {
    if(fileName == "") {
        fileName = QFileDialog::getOpenFileName(this, tr("Open image file"), "", tr("Supported Images (*.pgm *.ppm *.pbm *.jpeg *.jpg *.gif)"));
    } if(fileName != "") {
        QPixmap originalPixmap(fileName);

        inputImage = originalPixmap.toImage();
        width = inputImage.width();
        height = inputImage.height();
        int *matrix = new int [width*height];

        for(int j = 0; j < height; j++) {
            for(int i = 0; i < width; i++) {
                matrix[j*width+i] = qGray(inputImage.pixel(i,j));
            }
        }

        int* blurredMatrix = mcgbri004::applyGausianBlur(matrix, width, height, 1.4, 5);
        smoothedImage = QImage(width, height, QImage::Format_RGB32);
        mcgbri004::setQImage(smoothedImage, blurredMatrix, width, height);

        mcgbri004::EdgeDetection * edgeDetector = new mcgbri004::EdgeDetection(blurredMatrix, width, height);
        int* magnitudeMatrix = edgeDetector->getMagnitudeImageRef();
        magnitudeImage = QImage(width, height, QImage::Format_RGB32);
        mcgbri004::setQImage(magnitudeImage, magnitudeMatrix, width, height);

        int* edgeMatrix =  edgeDetector->getEdgeDetectionImageRef();
        edgeDetectionImage = QImage(width, height, QImage::Format_RGB32);
        mcgbri004::setQImage(edgeDetectionImage, edgeMatrix, width, height);
        delete[] blurredMatrix;

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

        int xOverlap = circleHoughTransform->getXOverlap();
        int yOverlap = circleHoughTransform->getYOverlap();
        accumulatorImages = new QImage[circleHoughTransform->totalRLength];
        int* imageMatrix = circleHoughTransform->getAccumulatorImageMatrix();
        for(int r = 0; r < circleHoughTransform->totalRLength; ++r) {
            accumulatorImages[r] = QImage(width + (2 * xOverlap), height + (2 * yOverlap), QImage::Format_RGB32);
            for(int j = 0; j < height + (2 * yOverlap); j++) {
                for(int i = 0; i < width + (2 * xOverlap); i++) {
                    if(imageMatrix[r*(width + (2 * xOverlap))*(height + (2 * yOverlap)) + j*(width + (2 * xOverlap))+i] > 0) {
                        accumulatorImages[r].setPixel(i, j, qRgb(255, 0, 0));
                    } else {
                        accumulatorImages[r].setPixel(i, j, qRgb(0, 0, 0));
                    }
                }
            }
        }

        filteredAccumulatorImages = new QImage[circleHoughTransform->totalRLength];
        int* filteredImageMatrix = circleHoughTransform->getFilteredAccumulatorImageMatrix();
        for(int r = 0; r < circleHoughTransform->totalRLength; ++r) {
            filteredAccumulatorImages[r] = QImage(width + (2 * xOverlap), height + (2 * yOverlap), QImage::Format_RGB32);
            for(int j = 0; j < height + (2 * yOverlap); j++) {
                for(int i = 0; i < width + (2 * xOverlap); i++) {
                    if(filteredImageMatrix[r*(width + (2 * xOverlap))*(height + (2 * yOverlap)) + j*(width + (2 * xOverlap))+i] > 0) {
                        filteredAccumulatorImages[r].setPixel(i, j, qRgb(255, 0, 0));
                    } else {
                        filteredAccumulatorImages[r].setPixel(i, j, qRgb(0, 0, 0));
                    }
                }
            }
        }

        QPixmap modifiedPixmap = QPixmap::fromImage(circleDetectionImage);
        prepareWindow(modifiedPixmap, false, 6);

        delete[] matrix;
        delete edgeDetector;
        delete circleHoughTransform;
    }
}

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

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open image file"), (baseName+baseExt).c_str(), tr("Netpbm (*.pgm *.ppm *.pbm);;GIF (*.gif);;JPEG (*.jpg *.jpeg)"), &defaultFilter);
    QString ext = QFileInfo(fileName).suffix();

    imageLabel->pixmap()->save(fileName, ext.toStdString().c_str());
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
    imageDescriptionLabel->setText("Gausian Blurred Image");
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
    QPixmap modifiedPixmap = QPixmap::fromImage(accumulatorImages[rLength - mcgbri004::CircleHoughTransform::rStart]); 
    prepareWindow(modifiedPixmap, true, 4, rLength);
    accumulatorImageAction->setChecked(true);
    imageDescriptionLabel->setText("Accumulator View");
}

void window::selectFilteredAccumulatorImage(int rLength) {
    QPixmap modifiedPixmap = QPixmap::fromImage(filteredAccumulatorImages[rLength - mcgbri004::CircleHoughTransform::rStart]);
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

void window::prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection) {
    prepareWindow(pixmap, isAccumulator, currentSelection, -1);
}

void window::selectCorrectAccumulatorImage(int rLength) {
    if(currentImageSelection == 4) {
        selectAccumulatorImage(rLength);
    } else if(currentImageSelection == 5) {
        selectFilteredAccumulatorImage(rLength);
    }
}

void window::prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection, int rLength) {
    if(isAccumulator) {
        std::string labelText = "Accumulator with radius set to " + std::to_string(rLength);
        sliderLabel->setText(labelText.c_str());
        resize(width + accumulatorWidthExtention, height + accumulatorHeightExtention);
    } else {
        resize(width + regularWidthExtention, height + regularHeightExtention);
    }
    currentImageSelection = currentSelection;
    setActionsVisibility(false);
    sliderLabel->setVisible(isAccumulator);
    accumulatorSlider->setVisible(isAccumulator);
    imageLabel->setPixmap(pixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
}

void window::setActionsVisibility(bool isVisible) {
    inputImageAction->setChecked(isVisible);
    smoothedImageAction->setChecked(isVisible);
    edgeDetectionImageAction->setChecked(isVisible);
    accumulatorImageAction->setChecked(isVisible);
    filteredAccumulatorImageAction->setChecked(isVisible);
    circleDetectionImageAction->setChecked(isVisible);
    magnitudeImageAction->setChecked(isVisible);
}
