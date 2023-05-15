TARGET = qmaemo5

QT += core-private gui-private
greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 7): \
    QT += theme_support-private
else: \
    QT += platformsupport-private

CONFIG += X11
CONFIG += link_pkgconfig
PKGCONFIG += hildon-fm-2 hildon-1

HEADERS += \
        qmaemo5dialoghelpers.h \
        qmaemo5theme.h

SOURCES += \
        main.cpp \
        qmaemo5dialoghelpers.cpp \
        qmaemo5theme.cpp \

PLUGIN_TYPE = platformthemes
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QMaemo5ThemePlugin
load(qt_plugin)
