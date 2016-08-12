// Brian Mc George
// MCGBRI004

#ifndef WINDOW_H_
#define WINDOW_H_

#include <QMainWindow>
#include <memory>
#include <vector>

// Forward declare class
class GLWidget;
class QAction;
class QMenu;
class QLabel;
class QScrollArea;
class QSlider;
class QVBoxLayout;

class window : public QMainWindow {
    Q_OBJECT
public:
    virtual ~window();
    explicit window(QWidget *parent = 0);

private slots:
    void open(QString &);
    void save();
    void selectInputImage();
    void selectSmoothedImage();
    void selectMagnitudeImage();    
    void selectEdgeDetectionImage();
    void selectAccumulatorImage(int rLength);
    void selectFilteredAccumulatorImage(int rLength);
    void selectCorrectAccumulatorImage(int rLength);
    void selectCircleDetectionImage();

private:
    const int regularWidthExtention = 20;
    const int regularHeightExtention = 60;
    const int accumulatorWidthExtention = 162;
    const int accumulatorHeightExtention = 300;
    QLabel *imageLabel;
    QScrollArea *imageScrollArea;
    int currentImageSelection;
    void addActions();
    void addMenus();
    void addConections();
    void setUpWidget();
    int height;
    int width;
    void setActionsVisibility(bool isVisible);
    void prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection);
    void prepareWindow(QPixmap & pixmap, bool isAccumulator, int currentSelection, int rLength);
    void setQImage(QImage & qImage, int* image, int imageXAxisLength, int imageYAxisLength);
    QMenu *fileMenu;
    QMenu *imageMenu;
    QAction *openAction;
    QAction *saveAction;
    QAction *inputImageAction;
    QAction *smoothedImageAction;
    QAction *magnitudeImageAction;
    QAction *edgeDetectionImageAction;
    QAction *accumulatorImageAction;
    QAction *filteredAccumulatorImageAction;
    QAction *circleDetectionImageAction;
    QWidget *qLayoutWidget;
    QImage inputImage;
    QImage smoothedImage;
    QImage magnitudeImage;
    QImage edgeDetectionImage;
    QImage *accumulatorImages;
    QImage *filteredAccumulatorImages;
    QImage circleDetectionImage;
    QLabel *imageDescriptionLabel;

    QLabel * sliderLabel;    
    QSlider *accumulatorSlider;
    QVBoxLayout *layout;  
    double scaleFactor;
};

#endif  // WINDOW_H_
