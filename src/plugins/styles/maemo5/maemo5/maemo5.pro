TARGET  = QtMaemo5
QPRO_PWD   = $$PWD

QT = core-private gui-private widgets-private dbus x11extras

CONFIG    += x11 link_pkgconfig
DEFINES   += QT_BUILD_MAEMO5_LIB QT_NO_USING_NAMESPACE QT_USE_GTK_PRIVATE

PKGCONFIG += gtk+-2.0

LIBS_PRIVATE += -L../maemo5style -lQMaemo5Style

# remove me
CONFIG += debug

load(qt_module)

HEADERS += \
    qmaemo5informationbox.h \
    qmaemo5abstractpickselector.h \
    qmaemo5abstractpickselector_p.h \
    qmaemo5listpickselector.h \
    qmaemo5listpickselector_p.h \
    qmaemo5datepickselector.h \
    qmaemo5datepickselector_p.h \
    qmaemo5timepickselector.h \
    qmaemo5timepickselector_p.h \
    qmaemo5editbar.h \
    qmaemo5editbar_p.h \
    qmaemo5valuebutton.h \
    qmaemo5valuebutton_p.h

SOURCES += \
    qmaemo5informationbox.cpp \
    qmaemo5abstractpickselector.cpp \
    qmaemo5listpickselector.cpp \
    qmaemo5datepickselector.cpp \
    qmaemo5datetimepickselector.cpp \
    qmaemo5timepickselector.cpp \
    qmaemo5editbar.cpp \
    qmaemo5valuebutton.cpp

INCLUDEPATH += INCLUDEPATH += ../../gtk2 ../maemo5style

#QMAKE_CXXFLAGS_RELEASE = -O0 -g
#QMAKE_CXXFLAGS = -O0 -g
