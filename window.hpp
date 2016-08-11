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
    void selectAccumulatorImage();
    void selectCircleDetectionImage();

private:
    void addActions();
    void addMenus();
    void addConections();
    void setUpWidget();
    int currentImageSelection;
    int height;
    int width;
    QMenu *fileMenu;
    QMenu *imageMenu;
    QAction *openAction;
    QAction *saveAction;
    QAction *inputImageAction;
    QAction *smoothedImageAction;
    QAction *magnitudeImageAction;
    QAction *edgeDetectionImageAction;
    QAction *accumulatorImageAction;
    QAction *circleDetectionImageAction;
    QWidget *qLayoutWidget;
    QImage inputImage;
    QImage smoothedImage;
    QImage magnitudeImage;
    QImage edgeDetectionImage;
    QImage *accumulatorImages;
    QImage circleDetectionImage;
    QLabel *imageDescriptionLabel;
    QLabel *imageLabel;
    QLabel * sliderLabel;
    QScrollArea *imageScrollArea;
    QSlider *accumulatorSlider;
    QVBoxLayout *layout;
    double scaleFactor;
    std::vector<window *> windows;
};

#endif  // WINDOW_H_
