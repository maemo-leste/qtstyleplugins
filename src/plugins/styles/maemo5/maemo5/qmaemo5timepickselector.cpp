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

#include <QTime>
#include <qmaemo5timepickselector.h>
#include <qmaemo5timepickselector_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMaemo5TimePickSelector
    \since 4.6
    \ingroup qtmaemo5
    \brief The QMaemo5TimePickSelector class can be used together with a
    QMaemo5ValueButton to allow the selection of a time.

    The Maemo5 time pick selector is a Maemo 5 specific class that is mainly
    used together with the so called "picker buttons" on this platform.
    The picker will handle all internal communication, such as setting the value
    of the picker button after the user has selected the new time.

    \sa QMaemo5AbstractPickSelector
*/

void QMaemo5TimePickSelectorPrivate::init()
{
    resolve();
    recreateSelector();
}

void QMaemo5TimePickSelectorPrivate::recreateSelector()
{
    if (selector)
        g_object_unref(selector);
    selector = hildon_time_selector_new_step(minuteStep);
    g_object_ref_sink(selector);
    hildon_time_selector_set_time(selector, time.hour(), time.minute());
}

void QMaemo5TimePickSelectorPrivate::emitSelected()
{
    Q_Q(QMaemo5TimePickSelector);

    guint hh = 0, hm = 0;
    hildon_time_selector_get_time(selector, &hh, &hm);
    time.setHMS(hh, hm, 0);
    emit q->selected(q->currentValueText());
}


/*!
    Constructs a new time pick selector with the given \a parent object.
*/
QMaemo5TimePickSelector::QMaemo5TimePickSelector(QObject *parent)
    : QMaemo5AbstractPickSelector(*new QMaemo5TimePickSelectorPrivate, parent)
{
    Q_D(QMaemo5TimePickSelector);
    d->init();
}

/*!
    \internal
*/
QMaemo5TimePickSelector::QMaemo5TimePickSelector(QMaemo5TimePickSelectorPrivate &dd, QObject *parent)
    : QMaemo5AbstractPickSelector(dd, parent)
{
    Q_D(QMaemo5TimePickSelector);
    d->init();
}

/*!
   Destroys the pick selector.
*/
QMaemo5TimePickSelector::~QMaemo5TimePickSelector()
{ }

/*!
    \property QMaemo5TimePickSelector::currentTime
    \brief The time displayed by the picker.

    This time is displayed when the pick selector is shown.  It is changed
    to the user selection after the user presses the accept button.
*/
QTime QMaemo5TimePickSelector::currentTime() const
{
    Q_D(const QMaemo5TimePickSelector);
    return d->time;
}

void QMaemo5TimePickSelector::setCurrentTime(const QTime &time)
{
    Q_D(QMaemo5TimePickSelector);
    if (time != d->time) {
        d->time = time;
        d->hildon_time_selector_set_time(d->selector, d->time.hour(), d->time.minute());
        emit selected(currentValueText());
    }
}

/*!
    \property QMaemo5TimePickSelector::minuteStep
    \brief The step used for the minute list.

    Depending on this setting, the list will have from 1 to 60 entries.
    For example, setting it to 15 will result in a minute list with the entries
    0, 15, 30, 45.

    Although setting the step to a value larger than 60 does not make any
    sense, the minute list will still be shown with one entry of 0.
*/
int QMaemo5TimePickSelector::minuteStep() const
{
    Q_D(const QMaemo5TimePickSelector);
    return d->minuteStep;
}

void QMaemo5TimePickSelector::setMinuteStep(int step)
{
    Q_D(QMaemo5TimePickSelector);
    if (step != d->minuteStep) {
        d->minuteStep = step;
        d->recreateSelector();
    }
}

/*!
    \reimp
*/
QWidget *QMaemo5TimePickSelector::widget(QWidget *parent)
{
    Q_D(QMaemo5TimePickSelector);
    return d->widget(parent);
}

/*! \reimp
*/
QString QMaemo5TimePickSelector::currentValueText() const
{
    Q_D(const QMaemo5TimePickSelector);
    return d->asText();
}

QT_END_NAMESPACE
