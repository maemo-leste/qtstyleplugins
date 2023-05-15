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

#include "qmaemo5theme.h"
#include "qmaemo5dialoghelpers.h"
#include <QVariant>

#undef signals
#include <hildon/hildon.h>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

const char *QMaemo5Theme::name = "maemo5";

QMaemo5Theme::QMaemo5Theme()
{
    // gtk_init will reset the Xlib error handler, and that causes
    // Qt applications to quit on X errors. Therefore, we need to manually restore it.
    int (*oldErrorHandler)(Display *, XErrorEvent *) = XSetErrorHandler(NULL);

    hildon_gtk_init(0, 0);

    XSetErrorHandler(oldErrorHandler);
}


bool QMaemo5Theme::usePlatformNativeDialog(DialogType type) const
{
    switch (type) {
    case ColorDialog:
        return false;
    case FileDialog:
        return true;
    case FontDialog:
        return false;
    default:
        return false;
    }
}

QPlatformDialogHelper *QMaemo5Theme::createPlatformDialogHelper(DialogType type) const
{
    switch (type) {
    /*case ColorDialog:
        return new QMaemo5ColorDialogHelper;*/
    case FileDialog:
        return new QMaemo5FileDialogHelper;
    /*case FontDialog:
        return new QMaemo5FontDialogHelper;*/
    default:
        return 0;
    }
}

QT_END_NAMESPACE
