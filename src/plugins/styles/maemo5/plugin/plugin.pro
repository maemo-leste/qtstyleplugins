TARGET  = qmaemo5style
PLUGIN_TYPE = styles
PLUGIN_CLASS_NAME = QMaemo5StylePlugin
load(qt_plugin)

QT = core-private gui-private widgets-private

CONFIG += link_pkgconfig debug
PKGCONFIG += hildon-1 hildon-fm-2 gtk+-2.0 x11

DEFINES += QT_NO_ANIMATION

SOURCES += plugin.cpp

include(../../shared/shared.pri)

INCLUDEPATH += ../module ../../gtk2
LIBS += -L../../../../../lib/ -lQt5Maemo5Style

OTHER_FILES += maemo5.json

QMAKE_CXXFLAGS_RELEASE = -O0 -g
QMAKE_CXXFLAGS = -O0 -g
