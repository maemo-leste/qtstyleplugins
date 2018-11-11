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

#ifndef QMAEMO5TIMEPICKSELECTOR_H
#define QMAEMO5TIMEPICKSELECTOR_H

#include <QtMaemo5/qmaemo5abstractpickselector.h>
#include <QtCore/qdatetime.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Maemo5)

class QMaemo5TimePickSelectorPrivate;

class Q_MAEMO5_EXPORT QMaemo5TimePickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT
    Q_PROPERTY(int minuteStep READ minuteStep WRITE setMinuteStep)
    Q_PROPERTY(QTime currentTime READ currentTime WRITE setCurrentTime)

public:
    explicit QMaemo5TimePickSelector(QObject *parent = 0);
    ~QMaemo5TimePickSelector();

    QWidget *widget(QWidget *parent);

    QTime currentTime() const;
    void setCurrentTime(const QTime &time);

    int minuteStep() const;
    void setMinuteStep(int step);

    QString currentValueText() const;

protected:
    QMaemo5TimePickSelector(QMaemo5TimePickSelectorPrivate &dd, QObject *parent = 0);

private:
    Q_DISABLE_COPY(QMaemo5TimePickSelector)
    Q_DECLARE_PRIVATE(QMaemo5TimePickSelector)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMAEMO5TIMEPICKSELECTOR_H
