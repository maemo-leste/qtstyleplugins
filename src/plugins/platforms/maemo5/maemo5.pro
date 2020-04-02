TARGET = qmaemo5

QT += \
    core-private gui-private \
    eventdispatcher_support-private fontdatabase_support-private

DEFINES += QT_NO_FOREACH

SOURCES =   main.cpp \
            qmaemo5integration.cpp
HEADERS =   qmaemo5integration.h

OTHER_FILES += maemo5.json

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QMaemo5IntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -

QMAKE_LFLAGS = $(QMAKE_LFLAGS) -lQt5XcbQpa
QMAKE_CXXFLAGS = $(QMAKE_CXXFLAGS) -Ixcb

load(qt_plugin)
