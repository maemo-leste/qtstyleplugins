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

#ifndef QMAEMO5VALUEBUTTON_H
#define QMAEMO5VALUEBUTTON_H

#include "QtMaemo5/maemo5global.h"

#include <QtWidgets/qpushbutton.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Maemo5)

class QMaemo5ValueButtonPrivate;
class QMaemo5AbstractPickSelector;

class Q_MAEMO5_EXPORT QMaemo5ValueButton : public QPushButton
{
    Q_OBJECT
    Q_ENUMS(ValueLayout)
    Q_PROPERTY(QString valueText READ valueText WRITE setValueText)
    Q_PROPERTY(ValueLayout valueLayout READ valueLayout WRITE setValueLayout)

public:
    explicit QMaemo5ValueButton(QWidget *parent = 0);
    explicit QMaemo5ValueButton(const QString &text, QWidget *parent = 0);
    QMaemo5ValueButton(const QIcon &icon, const QString &text, QWidget *parent = 0);
    ~QMaemo5ValueButton();

    QSize sizeHint() const;

    enum ValueLayout {
        ValueBesideText,
        ValueUnderText,
        ValueUnderTextCentered
    };

    QString valueText() const;
    ValueLayout valueLayout() const;
    void setValueLayout(ValueLayout layout);

    QMaemo5AbstractPickSelector *pickSelector() const;
    void setPickSelector(QMaemo5AbstractPickSelector *picker);

public Q_SLOTS:
    virtual void setValueText(const QString &text);

protected:
    void paintEvent(QPaintEvent *pe);
    QMaemo5ValueButton(QMaemo5ValueButtonPrivate &dd, QWidget* parent = 0);

    QScopedPointer<QMaemo5ValueButtonPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QMaemo5ValueButton)
    Q_DECLARE_PRIVATE(QMaemo5ValueButton)

    Q_PRIVATE_SLOT(d_func(), void _q_clicked());
    Q_PRIVATE_SLOT(d_func(), void _q_selected(const QString &));
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMAEMO5VALUEBUTTON_H
