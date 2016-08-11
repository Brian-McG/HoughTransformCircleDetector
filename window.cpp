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

window::window(QWidget *parent) : QMainWindow(parent), imageLabel(new QLabel), imageScrollArea(new QScrollArea), currentImageSelection(0) {
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
    delete circleDetectionImageAction;
    delete imageLabel;
    delete imageScrollArea;
    delete imageMenu;
    delete fileMenu;
    delete accumulatorSlider;
    delete layout;
    delete sliderLabel;
    delete qLayoutWidget;
    //delete[] accumulatorImages;
    for (unsigned int i =0; i < windows.size(); ++i) {
        delete windows[i];
    }
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
    accumulatorSlider->setTickInterval(10);
    accumulatorSlider->setSingleStep(1);
    accumulatorSlider->setVisible(false);

    sliderLabel = new QLabel;
    sliderLabel->setText("Accumulator with radius set to 15");
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
    imageMenu->addAction(circleDetectionImageAction);
}

void window::addConections() {
    connect(openAction, &QAction::triggered, this, [this]{QString fileName = ""; open(fileName); });
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(inputImageAction, SIGNAL(triggered()), this, SLOT(selectInputImage()));
    connect(smoothedImageAction, SIGNAL(triggered()), this, SLOT(selectSmoothedImage()));
    connect(magnitudeImageAction, SIGNAL(triggered()), this, SLOT(selectMagnitudeImage()));
    connect(edgeDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectEdgeDetectionImage()));
    connect(accumulatorImageAction, SIGNAL(triggered()), this, SLOT(selectAccumulatorImage()));
    connect(circleDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectCircleDetectionImage()));
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

        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
            {
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
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
            {
                if(circleDetectedMatrix[j*width+i] != 0) {
                    circleDetectionImage.setPixel(i, j, qRgb(255, 0, 0));
                } else {
                    circleDetectionImage.setPixel(i, j, qRgb(matrix[j*width+i], matrix[j*width+i], matrix[j*width+i]));
                }
            }
        }

        accumulatorImages = new QImage[mcgbri004::CircleHoughTransform::rWindow];
        QImage *accumulatorImagesRef = circleHoughTransform->getAccumulatorImages();
        accumulatorImages[0] = accumulatorImagesRef[0];

        QPixmap modifiedPixmap = QPixmap::fromImage(circleDetectionImage);

        imageLabel->setPixmap(modifiedPixmap);
        imageLabel->resize(imageLabel->pixmap()->size());
        circleDetectionImageAction->setChecked(true);
        imageDescriptionLabel->setText("Circle Hough Transform Detected Image");
//        resize(width + 75, height + 100);
        resize(width + 20, height + 60);

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
        baseName = "edge_detection" + baseName;
    } else {
        baseName = "circle_detection" + baseName;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Open image file"), (baseName+baseExt).c_str(), tr("Netpbm (*.pgm *.ppm *.pbm);;GIF (*.gif);;JPEG (*.jpg *.jpeg)"), &defaultFilter);
    QString ext = QFileInfo(fileName).suffix();

    imageLabel->pixmap()->save(fileName, ext.toStdString().c_str());
}

void window::selectInputImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(inputImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 0;
    inputImageAction->setChecked(true);
    smoothedImageAction->setChecked(false);
    edgeDetectionImageAction->setChecked(false);
    accumulatorImageAction->setChecked(false);
    circleDetectionImageAction->setChecked(false);
    magnitudeImageAction->setChecked(false);
    sliderLabel->setVisible(false);
    accumulatorSlider->setVisible(false);
    imageDescriptionLabel->setText("Input Image");
    resize(width + 20, height + 60);
}

void window::selectSmoothedImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(smoothedImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 1;
    inputImageAction->setChecked(false);
    smoothedImageAction->setChecked(true);
    edgeDetectionImageAction->setChecked(false);
    accumulatorImageAction->setChecked(false);
    circleDetectionImageAction->setChecked(false);
    magnitudeImageAction->setChecked(false);
    sliderLabel->setVisible(false);
    accumulatorSlider->setVisible(false);
    imageDescriptionLabel->setText("Gausian Blurred Image");
    resize(width + 20, height + 60);
}

void window::selectMagnitudeImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(magnitudeImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 4;
    inputImageAction->setChecked(false);
    smoothedImageAction->setChecked(false);
    edgeDetectionImageAction->setChecked(false);
    circleDetectionImageAction->setChecked(false);
    accumulatorImageAction->setChecked(false);
    magnitudeImageAction->setChecked(true);
    sliderLabel->setVisible(false);
    accumulatorSlider->setVisible(false);
    imageDescriptionLabel->setText("Intensity Gradient Image using a 5x5 Sobel Filter");
    resize(width + 20, height + 60);
}

void window::selectEdgeDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(edgeDetectionImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 2;
    inputImageAction->setChecked(false);
    smoothedImageAction->setChecked(false);
    edgeDetectionImageAction->setChecked(true);
    accumulatorImageAction->setChecked(false);
    circleDetectionImageAction->setChecked(false);
    magnitudeImageAction->setChecked(false);
    sliderLabel->setVisible(false);
    accumulatorSlider->setVisible(false);
    imageDescriptionLabel->setText("Canny Edge Detected Image");
    resize(width + 20, height + 60);
}

void window::selectAccumulatorImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(accumulatorImages[0]);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 3;
    inputImageAction->setChecked(false);
    smoothedImageAction->setChecked(false);
    edgeDetectionImageAction->setChecked(false);
    accumulatorImageAction->setChecked(true);
    circleDetectionImageAction->setChecked(false);
    magnitudeImageAction->setChecked(false);
    sliderLabel->setVisible(true);
    accumulatorSlider->setVisible(true);
    imageDescriptionLabel->setText("Accumulator View");
    resize(width + 20, height + 112);
}

void window::selectCircleDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(circleDetectionImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 3;
    inputImageAction->setChecked(false);
    smoothedImageAction->setChecked(false);
    edgeDetectionImageAction->setChecked(false);
    accumulatorImageAction->setChecked(false);
    circleDetectionImageAction->setChecked(true);
    magnitudeImageAction->setChecked(false);
    sliderLabel->setVisible(false);
    accumulatorSlider->setVisible(false);
    imageDescriptionLabel->setText("Circle Hough Transform Detected Image");
    resize(width + 20, height + 60);
}

