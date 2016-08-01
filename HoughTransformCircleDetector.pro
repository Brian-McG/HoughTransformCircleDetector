HEADERS       = \
    gaussianblur.hpp \
    window.hpp

SOURCES       = main.cpp \
                window.cpp \
    gaussianblur.cpp
QT += core gui opengl widgets
#RESOURCES += resources.qrc
#QMAKE_CXXFLAGS += -DSHOULD_LOAD_SHADERS -std=c++11

QMAKE_CXXFLAGS += -std=c++11
# install
target.path = boom
INSTALLS += target
win32 {
    LIBS += -lgdi32 -luser32 -lshell32
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $${OUT_PWD}
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g

    copyfiles.commands = mkdir $${DESTDIR_WIN}\\images & xcopy /S /I /E /Y $${PWD_WIN}\\images $${DESTDIR_WIN}\\images

    RC_ICONS += icon-image-128.ico
}
macx {
    copydata.commands = cp -r $$PWD/images $$OUT_PWD
}
unix {
    copydata.commands = cp -r $$PWD/images $$OUT_PWD
}
QMAKE_EXTRA_TARGETS += copyfiles
POST_TARGETDEPS += copyfiles
