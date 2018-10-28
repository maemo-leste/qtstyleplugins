TEMPLATE = subdirs
SUBDIRS = \
    module \
    plugin

plugin.depends = module
