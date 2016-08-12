// Copyright[2015] <Brian Mc George>
// MCGBRI004

#include <QApplication>
#include "include/window.hpp"
#include "include/gaussianblur.hpp"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    window w;
    w.show();
    return a.exec();
}
