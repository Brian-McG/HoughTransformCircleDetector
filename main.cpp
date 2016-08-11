// Copyright[2015] <Brian Mc George>
// MCGBRI004

#include <QApplication>
#include "window.hpp"
#include "gaussianblur.hpp"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    window w;
//    w.resize(500, 550);
    w.show();
    return a.exec();
}
