// Copyright[2016] <Brian Mc George>
// MCGBRI004

#include <QAction>
#include <iostream>
#include <QMenuBar>
#include <QFileDialog>
#include <QLabel>
#include <QScrollArea>
#include <string>
#include "./window.hpp"

window::window(QWidget *parent) : QMainWindow(parent), imageLabel(new QLabel), imageScrollArea(new QScrollArea), currentImageSelection(0) {
    setUpWidget();
    addActions();
    addMenus();
    addConections();
}

window::~window() { 
    delete openAction;
    delete saveAction;
    delete inputImageAction;
    delete smoothedImageAction;
    delete edgeDetectionImageAction;
    delete circleDetectionImageAction;
    delete imageLabel;
    delete imageScrollArea;
    delete imageMenu;
    delete fileMenu;
    for (unsigned int i =0; i < windows.size(); ++i) {
        delete windows[i];
    }
}

void window::setUpWidget() {

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageScrollArea->setWidget(imageLabel);

    setCentralWidget(imageScrollArea);
    open();
    imageScrollArea->setVisible(true);
}

void window::addActions() {
    openAction = new QAction(tr("O&pen image"), this);
    openAction->setStatusTip(tr("Opens an image"));
    saveAction = new QAction(tr("S&ave image"), this);
    saveAction->setStatusTip(tr("Saves an image"));
    inputImageAction = new QAction(tr("I&nput Image"), this);
    inputImageAction->setStatusTip(tr("Sets image to input image"));
    smoothedImageAction = new QAction(tr("S&moothed Image"), this);
    smoothedImageAction->setStatusTip(tr("Sets image to the smoothed image"));
    edgeDetectionImageAction = new QAction(tr("E&dge Detection Image"), this);
    edgeDetectionImageAction->setStatusTip(tr("Sets image to the edge detection image"));
    circleDetectionImageAction = new QAction(tr("C&ircle Detection Image"), this);
    circleDetectionImageAction->setStatusTip(tr("Sets image to the circle detection image"));
}

void window::addMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    imageMenu = menuBar()->addMenu(tr("&Pick Image"));
    imageMenu->addAction(inputImageAction);
    imageMenu->addAction(smoothedImageAction);
    imageMenu->addAction(edgeDetectionImageAction);
    imageMenu->addAction(circleDetectionImageAction);
}

void window::addConections() {
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(inputImageAction, SIGNAL(triggered()), this, SLOT(selectInputImage()));
    connect(smoothedImageAction, SIGNAL(triggered()), this, SLOT(selectSmoothedImage()));
    connect(edgeDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectEdgeDetectionImage()));
    connect(circleDetectionImageAction, SIGNAL(triggered()), this, SLOT(selectCircleDetectionImage()));
}

void window::open() {
    // Override filename to make testing quicker
    QString fileName = "images/testseq100000.pgm";
    QPixmap originalPixmap(fileName);

    inputImage = originalPixmap.toImage();
    int width = inputImage.width();
    int height = inputImage.height();
    int *matrix = new int [width*height];

    for(int j = 0; j < height; j++)
    {
      for(int i = 0; i < width; i++)
      {
        matrix[j*width+i] = qGray(inputImage.pixel(i,j));
      }
    }

    QImage modifiedImage(width, height, QImage::Format_RGB32);
    for(int j = 0; j < height; j++)
    {
      for(int i = 0; i < width; i++)
      {
        modifiedImage.setPixel(i, j, qRgb(matrix[j*width+i], matrix[j*width+i], matrix[j*width+i]));
      }
    }

    QPixmap modifiedPixmap = QPixmap::fromImage(modifiedImage);

    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());

    // QString defaultFilter = "Netpbm (*.pgm *.ppm *.pbm)";
    // QString fileName = QFileDialog::getOpenFileName(this, tr("Open image file"), "", tr("Netpbm (*.pgm *.ppm *.pbm);;GIF (*.gif);;JPEG (*.jpeg *.jpg)"), &defaultFilter);
    if (fileName != "") {

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
}

void window::selectSmoothedImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(smoothedImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 1;
}

void window::selectEdgeDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(edgeDetectionImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 2;

}

void window::selectCircleDetectionImage() {
    QPixmap modifiedPixmap = QPixmap::fromImage(circleDetectionImage);
    imageLabel->setPixmap(modifiedPixmap);
    imageLabel->resize(imageLabel->pixmap()->size());
    currentImageSelection = 3;

}

