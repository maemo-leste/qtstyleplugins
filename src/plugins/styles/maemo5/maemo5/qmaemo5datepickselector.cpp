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

#include <QDate>
#include <qmaemo5datepickselector.h>
#include <qmaemo5datepickselector_p.h>

QT_BEGIN_NAMESPACE

void QMaemo5DatePickSelectorPrivate::init()
{
    resolve();
    recreateSelector();
}

void QMaemo5DatePickSelectorPrivate::recreateSelector()
{
    if (selector)
        g_object_unref(selector);
    selector = hildon_date_selector_new_with_year_range(minYear, maxYear);
    g_object_ref_sink(selector);
    hildon_date_selector_select_current_date(selector, date.year(), date.month() - 1, date.day());
}

void QMaemo5DatePickSelectorPrivate::emitSelected()
{
    Q_Q(QMaemo5DatePickSelector);

    guint hy = 0, hm = 0, hd = 0;
    hildon_date_selector_get_date(selector, &hy, &hm, &hd);
    date.setDate(hy, hm + 1, hd);
    emit q->selected(q->currentValueText());
}

/*!
    \class QMaemo5DatePickSelector
    \since 4.6
    \ingroup qtmaemo5
    \brief The QMaemo5DatePickSelector class can be used together with a
    QMaemo5ValueButton to allow the selection of a date.

    The Maemo5 date pick selector is a Maemo 5 specific class that is mainly
    used together with the so called "picker buttons" on this platform.
    The picker will handle all internal communication, such as setting the value
    of the picker button after the user has selected the new date.

    \sa QMaemo5AbstractPickSelector
*/

/*!
    Constructs a new date pick selector with the given \a parent object.
*/
QMaemo5DatePickSelector::QMaemo5DatePickSelector(QObject *parent)
    : QMaemo5AbstractPickSelector(*new QMaemo5DatePickSelectorPrivate, parent)
{
    Q_D(QMaemo5DatePickSelector);
    d->init();
}

/*! \internal
 */
QMaemo5DatePickSelector::QMaemo5DatePickSelector(QMaemo5DatePickSelectorPrivate &dd, QObject *parent)
    : QMaemo5AbstractPickSelector(dd, parent)
{
    Q_D(QMaemo5DatePickSelector);
    d->init();
}

/*!
   Destroys the pick selector.
*/
QMaemo5DatePickSelector::~QMaemo5DatePickSelector()
{ }

/*!
    \property QMaemo5DatePickSelector::minimumYear
    \brief The minimum year

    This property holds the minimum year a user can choose.
*/

/*!
    \property QMaemo5DatePickSelector::maximumYear
    \brief The maximum year

    This property holds the maximum year a user can choose.
*/

/*!
    \property QMaemo5DatePickSelector::currentDate
    \brief The currently selected date

    This property holds the date that is currently selected.
*/

QDate QMaemo5DatePickSelector::currentDate() const
{
    Q_D(const QMaemo5DatePickSelector);
    return d->date;
}

void QMaemo5DatePickSelector::setCurrentDate(const QDate &date)
{
    Q_D(QMaemo5DatePickSelector);
    if (date != d->date) {
        d->date = date;
        d->hildon_date_selector_select_current_date(d->selector, d->date.year(), d->date.month() - 1, d->date.day());
        emit selected(currentValueText());
    }
}

int QMaemo5DatePickSelector::minimumYear() const
{
    Q_D(const QMaemo5DatePickSelector);
    return d->minYear;
}

int QMaemo5DatePickSelector::maximumYear() const
{
    Q_D(const QMaemo5DatePickSelector);
    return d->maxYear;
}

void QMaemo5DatePickSelector::setMinimumYear(int y)
{
    Q_D(QMaemo5DatePickSelector);
    if (y != d->minYear) {
        d->minYear = y;
        d->recreateSelector();
    }
}

void QMaemo5DatePickSelector::setMaximumYear(int y)
{
    Q_D(QMaemo5DatePickSelector);
    if (y != d->maxYear) {
        d->maxYear = y;
        d->recreateSelector();
    }
}

/*! \reimp
*/
QWidget *QMaemo5DatePickSelector::widget(QWidget *parent)
{
    Q_D(QMaemo5DatePickSelector);
    return d->widget(parent);
}

/*! \reimp
*/
QString QMaemo5DatePickSelector::currentValueText() const
{
    Q_D(const QMaemo5DatePickSelector);
    return d->asText();
}

QT_END_NAMESPACE
