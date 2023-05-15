TEMPLATE = subdirs

packagesExist(gtk+-2.0) {
    SUBDIRS += gtk2
}

packagesExist(hildon-fm-2) {
    SUBDIRS += maemo5
}
