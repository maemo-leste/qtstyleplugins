TEMPLATE = subdirs
SUBDIRS = \
    maemo5style \
    maemo5 \
    plugin

maemo5.depends = maemo5style
plugin.depends = maemo5
