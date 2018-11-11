/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QLibrary>
#include <QtWidgets/QApplication>
#include <QAbstractButton>
#include <QX11Info>
#include <QtDebug>

#include <qmaemo5datetimepickselector_p.h>

#include <X11/Xlib.h>


QT_BEGIN_NAMESPACE

Ptr_hildon_picker_dialog_new QMaemo5DateTimePickSelectorPrivate::hildon_picker_dialog_new = 0;
Ptr_hildon_picker_dialog_set_selector QMaemo5DateTimePickSelectorPrivate::hildon_picker_dialog_set_selector = 0;

Ptr_hildon_touch_selector_get_current_text QMaemo5DateTimePickSelectorPrivate::hildon_touch_selector_get_current_text = 0;

Ptr_hildon_date_selector_new_with_year_range QMaemo5DateTimePickSelectorPrivate::hildon_date_selector_new_with_year_range = 0;
Ptr_hildon_date_selector_get_date QMaemo5DateTimePickSelectorPrivate::hildon_date_selector_get_date = 0;
Ptr_hildon_date_selector_select_current_date QMaemo5DateTimePickSelectorPrivate::hildon_date_selector_select_current_date = 0;

Ptr_hildon_time_selector_new_step QMaemo5DateTimePickSelectorPrivate::hildon_time_selector_new_step = 0;
Ptr_hildon_time_selector_get_time QMaemo5DateTimePickSelectorPrivate::hildon_time_selector_get_time = 0;
Ptr_hildon_time_selector_set_time QMaemo5DateTimePickSelectorPrivate::hildon_time_selector_set_time = 0;

Ptr_gtk_widget_realize QMaemo5DateTimePickSelectorPrivate::gtk_widget_realize = 0;
Ptr_gtk_dialog_run QMaemo5DateTimePickSelectorPrivate::gtk_dialog_run = 0;
Ptr_gtk_widget_destroy QMaemo5DateTimePickSelectorPrivate::gtk_widget_destroy = 0;
Ptr_gtk_window_set_title QMaemo5DateTimePickSelectorPrivate::gtk_window_set_title = 0;

Ptr_gdk_x11_window_set_user_time QMaemo5DateTimePickSelectorPrivate::gdk_x11_window_set_user_time = 0;
Ptr_gdk_x11_drawable_get_xid QMaemo5DateTimePickSelectorPrivate::gdk_x11_drawable_get_xid = 0;
Ptr_gdk_x11_drawable_get_xdisplay QMaemo5DateTimePickSelectorPrivate::gdk_x11_drawable_get_xdisplay = 0;


QMaemo5DateTimePickSelectorPrivate::QMaemo5DateTimePickSelectorPrivate()
    : selector(0)
{ }

QMaemo5DateTimePickSelectorPrivate::~QMaemo5DateTimePickSelectorPrivate()
{
    if (selector)
        g_object_unref(selector);
}

QString QMaemo5DateTimePickSelectorPrivate::asText() const
{
    char *ptr = reinterpret_cast<char *>(hildon_touch_selector_get_current_text(selector));
    QString str = QString::fromUtf8(ptr);
    g_free(ptr);
    return str;
}

void QMaemo5DateTimePickSelectorPrivate::resolve()
{
    static bool resolved = false;
    if (!resolved) {
        QLibrary libgtk(QLatin1String("gtk-x11-2.0"), 0, 0);
        QLibrary libhildon(QLatin1String("hildon-1"), 0, 0);

        hildon_picker_dialog_new = (Ptr_hildon_picker_dialog_new)libhildon.resolve("hildon_picker_dialog_new");
        hildon_picker_dialog_set_selector = (Ptr_hildon_picker_dialog_set_selector)libhildon.resolve("hildon_picker_dialog_set_selector");

        hildon_touch_selector_get_current_text = (Ptr_hildon_touch_selector_get_current_text)libhildon.resolve("hildon_touch_selector_get_current_text");

        hildon_date_selector_new_with_year_range = (Ptr_hildon_date_selector_new_with_year_range)libhildon.resolve("hildon_date_selector_new_with_year_range");
        hildon_date_selector_get_date = (Ptr_hildon_date_selector_get_date)libhildon.resolve("hildon_date_selector_get_date");
        hildon_date_selector_select_current_date = (Ptr_hildon_date_selector_select_current_date)libhildon.resolve("hildon_date_selector_select_current_date");

        hildon_time_selector_new_step = (Ptr_hildon_time_selector_new_step)libhildon.resolve("hildon_time_selector_new_step");
        hildon_time_selector_get_time = (Ptr_hildon_time_selector_get_time)libhildon.resolve("hildon_time_selector_get_time");
        hildon_time_selector_set_time = (Ptr_hildon_time_selector_set_time)libhildon.resolve("hildon_time_selector_set_time");

        gtk_widget_destroy = (Ptr_gtk_widget_destroy)libgtk.resolve("gtk_widget_destroy");
        gtk_widget_realize = (Ptr_gtk_widget_realize)libgtk.resolve("gtk_widget_realize");
        gtk_dialog_run = (Ptr_gtk_dialog_run)libgtk.resolve("gtk_dialog_run");
        gtk_window_set_title = (Ptr_gtk_window_set_title)libgtk.resolve("gtk_window_set_title");

        gdk_x11_window_set_user_time = (Ptr_gdk_x11_window_set_user_time)libgtk.resolve("gdk_x11_window_set_user_time");
        gdk_x11_drawable_get_xid = (Ptr_gdk_x11_drawable_get_xid)libgtk.resolve("gdk_x11_drawable_get_xid");
        gdk_x11_drawable_get_xdisplay = (Ptr_gdk_x11_drawable_get_xdisplay)libgtk.resolve("gdk_x11_drawable_get_xdisplay");

        resolved = true;
    }
}

class DateTimePicker : public QWidget {
public:
    DateTimePicker(QMaemo5DateTimePickSelectorPrivate *picker, QWidget *parent)
        : QWidget(parent, Qt::Window), d(picker)
    {
        // we can't use QApplicationPrivate::enterModal(), so have to show a
        // dummy modal window
        setWindowModality(Qt::WindowModal);
        setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint);
        setGeometry(0, 0, 0, 0);
        setFocusPolicy(Qt::NoFocus);
    }

    void setVisible(bool b)
    {
        if (b) {
            GtkWidget *dialog = d->hildon_picker_dialog_new(NULL);
            if (!dialog) {
                qWarning("QMaemo5(Date|Time)PickSelector could not create a HildonPickerDialog object.");
                return;
            }

            d->hildon_picker_dialog_set_selector(dialog, d->selector);
            d->gtk_widget_realize(dialog);
            if (!dialog->window) {
                qWarning("QMaemo5(Date|Time)PickSelector could not realize a HildonPickerDialog object.");
                return;
            }
            QWidget *modalFor = parentWidget() ? parentWidget()->window() : qApp->activeWindow();
            if (modalFor) {
                XSetTransientForHint(d->gdk_x11_drawable_get_xdisplay(dialog->window),
                                     d->gdk_x11_drawable_get_xid(dialog->window),
                                     modalFor->winId());
            }
            d->gdk_x11_window_set_user_time (dialog->window, QX11Info::appUserTime());

            if (QAbstractButton *pb = qobject_cast<QAbstractButton *>(parentWidget()))
                d->gtk_window_set_title(dialog, pb->text().toUtf8().constData());

            setAttribute(Qt::WA_NoChildEventsForParent, true);

            QWidget::setVisible(true); // should really be QApplicationPrivate::enterModal(this);

            if (d->gtk_dialog_run(dialog) == GTK_RESPONSE_OK)
                d->emitSelected();

            QWidget::setVisible(false); // should really be QApplicationPrivate::leaveModal(this);

            d->gtk_widget_destroy (dialog);
            d->selector = 0;
            d->recreateSelector();
        }
    }

private:
    QMaemo5DateTimePickSelectorPrivate *d;
};

QWidget *QMaemo5DateTimePickSelectorPrivate::widget(QWidget *parent)
{
    return new DateTimePicker(this, parent);
}

QT_END_NAMESPACE
