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

#include <qmaemo5abstractpickselector.h>
#include <qmaemo5abstractpickselector_p.h>

#include "qvariant.h"

QT_BEGIN_NAMESPACE


/*! 
    \class QMaemo5AbstractPickSelector
    \since 4.6
    \ingroup qtmaemo5
    \brief The QMaemo5AbstractPickSelector class provides the basic functionality for a class
    providing values for a QMaemo5ValueButton.

    To implement a pick selector you need to implement the widget() and
    currentValueText() functions.

    \sa QMaemo5ValueButton
*/

/*!
    Constructs a new abstract pick selector with the given \a parent object.
*/
QMaemo5AbstractPickSelector::QMaemo5AbstractPickSelector(QObject *parent)
    : QObject(parent), d_ptr(new QMaemo5AbstractPickSelectorPrivate())
{
    Q_D(QMaemo5AbstractPickSelector);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    \internal
*/
QMaemo5AbstractPickSelector::QMaemo5AbstractPickSelector(QMaemo5AbstractPickSelectorPrivate &dd, QObject *parent)
    : QObject(parent), d_ptr(&dd)
{
    Q_D(QMaemo5AbstractPickSelector);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
   Destroys the pick selector.
*/
QMaemo5AbstractPickSelector::~QMaemo5AbstractPickSelector()
{
}

/*! 
    \fn QWidget *QMaemo5AbstractPickSelector::widget(QWidget *parent)

    Returns a widget that will be displayed when the associated QMaemo5ValueButton is pressed.

    To be compliant with the Maemo 5 UI guidelines, a reimplementation of this function
    should return an instance of a QDialog subclass with the \a parent widget specified.

    After the value has been selected, this widget should emit the selected signal and
    then hide itself.
*/

/*! 
    \fn QString QMaemo5AbstractPickSelector::currentValueText() const

    Returns the current text of this selector. This value is used as value text
    on a QMaemo5ValueButton.

    \sa QMaemo5ValueButton::valueText
*/

/*!
    \fn void QMaemo5AbstractPickSelector::selected(const QString &valueText)

    This signal is sent when the user chooses an item in the pick selector.
    The \a valueText parameter is the same as the return value of the
    currentValueText() function.

    \sa currentValueText()
*/
QT_END_NAMESPACE
