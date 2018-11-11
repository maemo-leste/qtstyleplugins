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

#ifndef QMAEMO5EDITBAR_H
#define QMAEMO5EDITBAR_H

#include "QtMaemo5/maemo5global.h"

#include <QtWidgets/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Maemo5)

class QMaemo5EditBarPrivate;
class QAbstractButton;

class Q_MAEMO5_EXPORT QMaemo5EditBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    explicit QMaemo5EditBar(QWidget *parent = 0);
    explicit QMaemo5EditBar(const QString &text, QWidget *parent = 0);
    ~QMaemo5EditBar();

    QString text() const;
    void addButton(QAbstractButton *b);
    void removeButton(QAbstractButton *b);
    QList<QAbstractButton *> buttons() const;

public Q_SLOTS:
    void setText(const QString &text);

protected:
    void paintEvent(QPaintEvent *pe);

    QMaemo5EditBar(QMaemo5EditBarPrivate &dd, QWidget* parent = 0);

    QScopedPointer<QMaemo5EditBarPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QMaemo5EditBar)
    Q_DECLARE_PRIVATE(QMaemo5EditBar)

    Q_PRIVATE_SLOT(d_func(), void _q_backClicked());
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMAEMO5EDITBAR_H
