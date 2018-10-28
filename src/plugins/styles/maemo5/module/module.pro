TARGET  = QtMaemo5Style
QPRO_PWD   = $$PWD

QT = core-private gui-private widgets-private

CONFIG += link_pkgconfig debug
PKGCONFIG += hildon-1 hildon-fm-2 gtk+-2.0 x11

DEFINES += QT_NO_ANIMATION

load(qt_module)

HEADERS += \
        ../../gtk2/qgtkstyle_p.h \
        ../../gtk2/qgtkstyle_p_p.h \
        ../../gtk2/qgtkpainter_p.h \
        ../../gtk2/qgtk2painter_p.h \
        ../../cleanlooks/qcleanlooksstyle.h \
        qmaemo5style.h \
        qmaemo5style_p.h
SOURCES += \
        ../../gtk2/qgtkstyle.cpp \
        ../../gtk2/qgtkstyle_p.cpp \
        ../../gtk2/qgtkpainter.cpp \
        ../../gtk2/qgtk2painter.cpp \
        ../../cleanlooks/qcleanlooksstyle.cpp \
        qmaemo5style.cpp 

include(../../shared/shared.pri)

INCLUDEPATH += ../../gtk2 ../../cleanlooks

QMAKE_CXXFLAGS_RELEASE = -O0 -g
QMAKE_CXXFLAGS = -O0 -g
