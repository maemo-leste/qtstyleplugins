/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmaemo5dialoghelpers.h"

#include <qeventloop.h>
#include <qwindow.h>
#include <qcolor.h>
#include <qdebug.h>
#include <qfont.h>

#include <private/qguiapplication_p.h>
#include <qpa/qplatformfontdatabase.h>

#undef signals
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <hildon/hildon-file-chooser-dialog.h>

QT_BEGIN_NAMESPACE

class QMaemo5Dialog : public QWindow
{
    Q_OBJECT

public:
    QMaemo5Dialog(GtkWidget *gtkWidget);
    ~QMaemo5Dialog();

    GtkDialog *gtkDialog() const;

    void exec();
    bool show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent);
    void hide();

Q_SIGNALS:
    void accept();
    void reject();

protected:
    static void onResponse(QMaemo5Dialog *dialog, int response);

private slots:
    void onParentWindowDestroyed();

private:
    GtkWidget *gtkWidget;
};

QMaemo5Dialog::QMaemo5Dialog(GtkWidget *gtkWidget) : gtkWidget(gtkWidget)
{
    g_signal_connect_swapped(G_OBJECT(gtkWidget), "response", G_CALLBACK(onResponse), this);
    g_signal_connect(G_OBJECT(gtkWidget), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
}

QMaemo5Dialog::~QMaemo5Dialog()
{
    gtk_clipboard_store(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
    gtk_widget_destroy(gtkWidget);
}

GtkDialog *QMaemo5Dialog::gtkDialog() const
{
    return GTK_DIALOG(gtkWidget);
}

void QMaemo5Dialog::exec()
{
    if (modality() == Qt::ApplicationModal) {
        // block input to the whole app, including other GTK dialogs
        gtk_dialog_run(gtkDialog());
    } else {
        // block input to the window, allow input to other GTK dialogs
        QEventLoop loop;
        connect(this, SIGNAL(accept()), &loop, SLOT(quit()));
        connect(this, SIGNAL(reject()), &loop, SLOT(quit()));
        loop.exec();
    }
}

bool QMaemo5Dialog::show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent)
{
    if (parent) {
        connect(parent, &QWindow::destroyed, this, &QMaemo5Dialog::onParentWindowDestroyed,
                Qt::UniqueConnection);
    }
    setParent(parent);
    setFlags(flags);
    setModality(modality);

    gtk_widget_realize(gtkWidget); // creates X window

    if (parent) {
        XSetTransientForHint(gdk_x11_drawable_get_xdisplay(gtkWidget->window),
                             gdk_x11_drawable_get_xid(gtkWidget->window),
                             parent->winId());
    }

    if (modality != Qt::NonModal) {
        gdk_window_set_modal_hint(gtkWidget->window, true);
        QGuiApplicationPrivate::showModalWindow(this);
    }

    gtk_widget_show(gtkWidget);
    gdk_window_focus(gtkWidget->window, 0);
    return true;
}

void QMaemo5Dialog::hide()
{
    QGuiApplicationPrivate::hideModalWindow(this);
    gtk_widget_hide(gtkWidget);
}

void QMaemo5Dialog::onResponse(QMaemo5Dialog *dialog, int response)
{
    if (response == GTK_RESPONSE_OK)
        emit dialog->accept();
    else
        emit dialog->reject();
}

void QMaemo5Dialog::onParentWindowDestroyed()
{
    // The QGtk2*DialogHelper classes own this object. Make sure the parent doesn't delete it.
    setParent(0);
}


QMaemo5FileDialogHelper::QMaemo5FileDialogHelper()
{
    d.reset(new QMaemo5Dialog(hildon_file_chooser_dialog_new(0, GTK_FILE_CHOOSER_ACTION_OPEN)));
    connect(d.data(), SIGNAL(accept()), this, SLOT(onAccepted()));
    connect(d.data(), SIGNAL(reject()), this, SIGNAL(reject()));

    g_signal_connect(GTK_FILE_CHOOSER(d->gtkDialog()), "selection-changed", G_CALLBACK(onSelectionChanged), this);
    g_signal_connect_swapped(GTK_FILE_CHOOSER(d->gtkDialog()), "current-folder-changed", G_CALLBACK(onCurrentFolderChanged), this);
}

QMaemo5FileDialogHelper::~QMaemo5FileDialogHelper()
{
}

bool QMaemo5FileDialogHelper::show(Qt::WindowFlags flags, Qt::WindowModality modality, QWindow *parent)
{
    _dir.clear();
    _selection.clear();

    applyOptions();
    return d->show(flags, modality, parent);
}

void QMaemo5FileDialogHelper::exec()
{
    d->exec();
}

void QMaemo5FileDialogHelper::hide()
{
    // After GtkFileChooserDialog has been hidden, gtk_file_chooser_get_current_folder()
    // & gtk_file_chooser_get_filenames() will return bogus values -> cache the actual
    // values before hiding the dialog
    _dir = directory();
    _selection = selectedFiles();

    d->hide();
}

bool QMaemo5FileDialogHelper::defaultNameFilterDisables() const
{
    return false;
}

void QMaemo5FileDialogHelper::setDirectory(const QUrl &directory)
{
    GtkDialog *gtkDialog = d->gtkDialog();
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(gtkDialog), directory.toLocalFile().toUtf8());
}

QUrl QMaemo5FileDialogHelper::directory() const
{
    // While GtkFileChooserDialog is hidden, gtk_file_chooser_get_current_folder()
    // returns a bogus value -> return the cached value before hiding
    if (!_dir.isEmpty())
        return _dir;

    QString ret;
    GtkDialog *gtkDialog = d->gtkDialog();
    gchar *folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(gtkDialog));
    if (folder) {
        ret = QString::fromUtf8(folder);
        g_free(folder);
    }
    return QUrl::fromLocalFile(ret);
}

void QMaemo5FileDialogHelper::selectFile(const QUrl &filename)
{
    GtkDialog *gtkDialog = d->gtkDialog();
    if (options()->acceptMode() == QFileDialogOptions::AcceptSave) {
        QFileInfo fi(filename.toLocalFile());
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(gtkDialog), fi.path().toUtf8());
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(gtkDialog), fi.fileName().toUtf8());
    } else {
        gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(gtkDialog), filename.toLocalFile().toUtf8());
    }
}

QList<QUrl> QMaemo5FileDialogHelper::selectedFiles() const
{
    // While GtkFileChooserDialog is hidden, gtk_file_chooser_get_filenames()
    // returns a bogus value -> return the cached value before hiding
    if (!_selection.isEmpty())
        return _selection;

    QList<QUrl> selection;
    GtkDialog *gtkDialog = d->gtkDialog();
    GSList *filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(gtkDialog));
    for (GSList *it  = filenames; it; it = it->next)
        selection += QUrl::fromLocalFile(QString::fromUtf8((const char*)it->data));
    g_slist_free(filenames);
    return selection;
}

void QMaemo5FileDialogHelper::setFilter()
{
    applyOptions();
}

void QMaemo5FileDialogHelper::selectNameFilter(const QString &filter)
{
    GtkFileFilter *gtkFilter = _filters.value(filter);
    if (gtkFilter) {
        GtkDialog *gtkDialog = d->gtkDialog();
        gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(gtkDialog), gtkFilter);
    }
}

QString QMaemo5FileDialogHelper::selectedNameFilter() const
{
    GtkDialog *gtkDialog = d->gtkDialog();
    GtkFileFilter *gtkFilter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(gtkDialog));
    return _filterNames.value(gtkFilter);
}

void QMaemo5FileDialogHelper::onAccepted()
{
    emit accept();

    QString filter = selectedNameFilter();
    if (filter.isEmpty())
        emit filterSelected(filter);

    QList<QUrl> files = selectedFiles();
    emit filesSelected(files);
    if (files.count() == 1)
        emit fileSelected(files.first());
}

void QMaemo5FileDialogHelper::onSelectionChanged(GtkDialog *gtkDialog, QMaemo5FileDialogHelper *helper)
{
    QString selection;
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gtkDialog));
    if (filename) {
        selection = QString::fromUtf8(filename);
        g_free(filename);
    }
    emit helper->currentChanged(QUrl::fromLocalFile(selection));
}

void QMaemo5FileDialogHelper::onCurrentFolderChanged(QMaemo5FileDialogHelper *dialog)
{
    emit dialog->directoryEntered(dialog->directory());
}

static GtkFileChooserAction gtkFileChooserAction(const QSharedPointer<QFileDialogOptions> &options)
{
    switch (options->fileMode()) {
    case QFileDialogOptions::AnyFile:
    case QFileDialogOptions::ExistingFile:
    case QFileDialogOptions::ExistingFiles:
        if (options->acceptMode() == QFileDialogOptions::AcceptOpen)
            return GTK_FILE_CHOOSER_ACTION_OPEN;
        else
            return GTK_FILE_CHOOSER_ACTION_SAVE;
    case QFileDialogOptions::Directory:
    case QFileDialogOptions::DirectoryOnly:
    default:
        if (options->acceptMode() == QFileDialogOptions::AcceptOpen)
            return GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
        else
            return GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER;
    }
}

void QMaemo5FileDialogHelper::applyOptions()
{
    GtkDialog *gtkDialog = d->gtkDialog();
    const QSharedPointer<QFileDialogOptions> &opts = options();

    gtk_window_set_title(GTK_WINDOW(gtkDialog), opts->windowTitle().toUtf8());
    gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(gtkDialog), true);

    const GtkFileChooserAction action = gtkFileChooserAction(opts);
    gtk_file_chooser_set_action(GTK_FILE_CHOOSER(gtkDialog), action);

    const bool selectMultiple = opts->fileMode() == QFileDialogOptions::ExistingFiles;
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(gtkDialog), selectMultiple);

    const bool confirmOverwrite = !opts->testOption(QFileDialogOptions::DontConfirmOverwrite);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(gtkDialog), confirmOverwrite);

    const QStringList nameFilters = opts->nameFilters();
    if (!nameFilters.isEmpty())
        setNameFilters(nameFilters);

    if (opts->initialDirectory().isLocalFile())
        setDirectory(opts->initialDirectory());

    foreach (const QUrl &filename, opts->initiallySelectedFiles())
        selectFile(filename);

    const QString initialNameFilter = opts->initiallySelectedNameFilter();
    if (!initialNameFilter.isEmpty())
        selectNameFilter(initialNameFilter);
}

void QMaemo5FileDialogHelper::setNameFilters(const QStringList &filters)
{
    GtkDialog *gtkDialog = d->gtkDialog();
    foreach (GtkFileFilter *filter, _filters)
        gtk_file_chooser_remove_filter(GTK_FILE_CHOOSER(gtkDialog), filter);

    _filters.clear();
    _filterNames.clear();

    foreach (const QString &filter, filters) {
        GtkFileFilter *gtkFilter = gtk_file_filter_new();
        const QString name = filter.left(filter.indexOf(QLatin1Char('(')));
        const QStringList extensions = cleanFilterList(filter);

        gtk_file_filter_set_name(gtkFilter, name.isEmpty() ? extensions.join(QStringLiteral(", ")).toUtf8() : name.toUtf8());
        foreach (const QString &ext, extensions)
            gtk_file_filter_add_pattern(gtkFilter, ext.toUtf8());

        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(gtkDialog), gtkFilter);

        _filters.insert(filter, gtkFilter);
        _filterNames.insert(gtkFilter, filter);
    }
}
QT_END_NAMESPACE

#include "qmaemo5dialoghelpers.moc"
