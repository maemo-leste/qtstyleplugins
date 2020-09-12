TEMPLATE = lib
CONFIG += staticlib

QT = core-private gui-private widgets-private x11extras

CONFIG    += x11 link_pkgconfig
DEFINES   += QT_BUILD_MAEMO5_LIB QT_NO_USING_NAMESPACE QT_USE_GTK_PRIVATE

PKGCONFIG += hildon-1 hildon-fm-2 gtk+-2.0 x11

DEFINES += QT_NO_ANIMATION

SOURCES += \
    ../../gtk2/qgtkstyle.cpp \
    ../../gtk2/qgtkstyle_p.cpp \
    ../../gtk2/qgtkpainter.cpp \
    ../../gtk2/qgtk2painter.cpp \
    ../../cleanlooks/qcleanlooksstyle.cpp \
    qmaemo5style.cpp

HEADERS += \
    ../../gtk2/qgtkstyle_p.h \
    ../../gtk2/qgtkstyle_p_p.h \
    ../../gtk2/qgtkpainter_p.h \
    ../../gtk2/qgtk2painter_p.h \
    ../../cleanlooks/qcleanlooksstyle.h \
    qmaemo5style.h \
    qmaemo5style_p.h

include(../../shared/shared.pri)

headers.files = qmaemo5style.h \
                ../../gtk2/qgtkstyle_p.h \
                QMaemo5Style
headers.path = $$[QT_INSTALL_HEADERS]/QtMaemo5

INSTALLS += headers

INCLUDEPATH += ../../gtk2 ../../cleanlooks
