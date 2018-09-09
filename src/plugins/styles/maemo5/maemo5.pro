TARGET  = qmaemo5style
PLUGIN_TYPE = styles
PLUGIN_CLASS_NAME = QMaemo5StylePlugin
load(qt_plugin)

QT = core-private gui-private widgets-private

#QT = core-private gui-private widgets widgets-private x11extras

CONFIG += link_pkgconfig
PKGCONFIG += hildon-1 hildon-fm-2 gtk+-2.0 x11

#HEADERS += qgtk2painter_p.h qgtkglobal_p.h qgtkpainter_p.h qgtkstyle_p.h qgtkstyle_p_p.h
#SOURCES += qgtk2painter.cpp qgtkpainter.cpp qgtkstyle.cpp qgtkstyle_p.cpp plugin.cpp
DEFINES += QT_NO_ANIMATION

HEADERS += \
        ../gtk2/qgtkstyle_p.h \
        ../gtk2/qgtkstyle_p_p.h \
        ../gtk2/qgtkpainter_p.h \
        ../gtk2/qgtk2painter_p.h \
        ../cleanlooks/qcleanlooksstyle.h \
        qmaemo5style.h \
        qmaemo5style_p.h
SOURCES += \
        ../gtk2/qgtkstyle.cpp \
        ../gtk2/qgtkstyle_p.cpp \
        ../gtk2/qgtkpainter.cpp \
        ../gtk2/qgtk2painter.cpp \
        ../cleanlooks/qcleanlooksstyle.cpp \
        qmaemo5style.cpp \
        plugin.cpp

include(../shared/shared.pri)

INCLUDEPATH += ../gtk2 ../cleanlooks

OTHER_FILES += maemo5.json

QMAKE_CXXFLAGS_RELEASE = -O0 -g
QMAKE_CXXFLAGS = -O0 -g
