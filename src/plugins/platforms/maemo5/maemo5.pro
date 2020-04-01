TARGET = qmaemo5

QT += \
    core-private gui-private \
    eventdispatcher_support-private fontdatabase_support-private

DEFINES += QT_NO_FOREACH

SOURCES =   main.cpp \
            qminimalintegration.cpp
HEADERS =   qminimalintegration.h

OTHER_FILES += minimal.json

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QMaemo5IntegrationPlugin
!equals(TARGET, $$QT_DEFAULT_QPA_PLUGIN): PLUGIN_EXTENDS = -

QMAKE_LFLAGS = $(QMAKE_LFLAGS) -lQt5XcbQpa

load(qt_plugin)
