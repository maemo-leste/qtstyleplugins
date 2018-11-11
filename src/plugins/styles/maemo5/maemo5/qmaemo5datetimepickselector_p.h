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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QString>
#undef signals // Collides with GTK stymbols
#include <X11/Xlib.h>
#include <gtk/gtk.h>

QT_BEGIN_NAMESPACE

typedef unsigned long XID;

typedef GtkWidget *(*Ptr_hildon_picker_dialog_new)(GtkWidget *);
typedef void (*Ptr_hildon_picker_dialog_set_selector)(GtkWidget *, void *);

typedef gchar *(*Ptr_hildon_touch_selector_get_current_text)(void *);

typedef void *(*Ptr_hildon_date_selector_new_with_year_range)(gint, gint);
typedef void (*Ptr_hildon_date_selector_get_date)(void *, guint *, guint *, guint *);
typedef gboolean (*Ptr_hildon_date_selector_select_current_date)(void *, guint, guint, guint);

typedef void *(*Ptr_hildon_time_selector_new_step)(guint);
typedef void (*Ptr_hildon_time_selector_get_time)(void *, guint *, guint *);
typedef gboolean (*Ptr_hildon_time_selector_set_time)(void *, guint, guint);

typedef void (*Ptr_gtk_widget_realize)(GtkWidget *);
typedef gint (*Ptr_gtk_dialog_run)(GtkWidget *);
typedef void (*Ptr_gtk_widget_destroy)(GtkWidget *);
typedef void (*Ptr_gtk_window_set_title)(GtkWidget *, const char *);

typedef void (*Ptr_gdk_x11_window_set_user_time)(GdkWindow *, guint32);
typedef XID (*Ptr_gdk_x11_drawable_get_xid)(GdkDrawable *);
typedef Display *(*Ptr_gdk_x11_drawable_get_xdisplay)(GdkDrawable *);


class QMaemo5DateTimePickSelectorPrivate
{
public:
    QMaemo5DateTimePickSelectorPrivate();
    virtual ~QMaemo5DateTimePickSelectorPrivate();

    QString asText() const;
    QWidget *widget(QWidget *parent);

    static void resolve();

    virtual void recreateSelector() = 0;
    virtual void emitSelected() = 0;

    void *selector;

    static Ptr_hildon_picker_dialog_new hildon_picker_dialog_new;
    static Ptr_hildon_picker_dialog_set_selector hildon_picker_dialog_set_selector;

    static Ptr_hildon_touch_selector_get_current_text hildon_touch_selector_get_current_text;

    static Ptr_hildon_date_selector_new_with_year_range hildon_date_selector_new_with_year_range;
    static Ptr_hildon_date_selector_get_date hildon_date_selector_get_date;
    static Ptr_hildon_date_selector_select_current_date hildon_date_selector_select_current_date;

    static Ptr_hildon_time_selector_new_step hildon_time_selector_new_step;
    static Ptr_hildon_time_selector_get_time hildon_time_selector_get_time;
    static Ptr_hildon_time_selector_set_time hildon_time_selector_set_time;

    static Ptr_gtk_widget_realize gtk_widget_realize;
    static Ptr_gtk_dialog_run gtk_dialog_run;
    static Ptr_gtk_widget_destroy gtk_widget_destroy;
    static Ptr_gtk_window_set_title gtk_window_set_title;

    static Ptr_gdk_x11_window_set_user_time gdk_x11_window_set_user_time;
    static Ptr_gdk_x11_drawable_get_xid gdk_x11_drawable_get_xid;
    static Ptr_gdk_x11_drawable_get_xdisplay gdk_x11_drawable_get_xdisplay;
};

QT_END_NAMESPACE
