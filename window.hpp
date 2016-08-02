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

class window : public QMainWindow {
  Q_OBJECT
 public:
  virtual ~window();
  explicit window(QWidget *parent = 0);

 private slots:
  void open();
  void save();
  void selectInputImage();
  void selectSmoothedImage();
  void selectEdgeDetectionImage();
  void selectCircleDetectionImage();

 private:
  void addActions();
  void addMenus();
  void addConections();
  void setUpWidget();
  int currentImageSelection;
  QMenu *fileMenu;
  QMenu *imageMenu;
  QAction *openAction;
  QAction *saveAction;
  QAction *inputImageAction;
  QAction *smoothedImageAction;
  QAction *edgeDetectionImageAction;
  QAction *circleDetectionImageAction;
  GLWidget *glWidget;
  QImage inputImage;
  QImage smoothedImage;
  QImage edgeDetectionImage;
  QImage circleDetectionImage;
  QLabel *imageLabel;
  QScrollArea *imageScrollArea;
  double scaleFactor;
  std::vector<window *> windows;
};

#endif  // WINDOW_H_
