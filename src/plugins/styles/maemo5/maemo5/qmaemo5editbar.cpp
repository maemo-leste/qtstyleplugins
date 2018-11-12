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
#include <QPainter>
#include <QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QStyle>
#include <QtWidgets/QStyleOption>

#include "qmaemo5style.h"

#include <qmaemo5editbar.h>
#include <qmaemo5editbar_p.h>

#include <QtDebug>

QT_BEGIN_NAMESPACE


QMaemo5BackButton::QMaemo5BackButton()
    : QPushButton()
{ }

QSize QMaemo5BackButton::sizeHint() const
{
    QSize s(112, 56);
    ensurePolished();
    if (qobject_cast<QMaemo5Style *>(style())) {
        s.setWidth(style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5EditBarBackButtonWidth)));
        s.setHeight(style()->pixelMetric(static_cast<QStyle::PixelMetric>(QMaemo5Style::PM_Maemo5EditBarBackButtonHeight)));
    }
    return s;
}

void QMaemo5BackButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    if (qobject_cast<QMaemo5Style *>(style())) {
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(QMaemo5Style::PE_Maemo5EditBarBackButton),
                               &option, &painter, this);
    }
}

void QMaemo5EditBarPrivate::init()
{
    Q_Q(QMaemo5EditBar);
    hbox = new QHBoxLayout(q);
    label = new QLabel();
    hbox->setSpacing(8);
    hbox->setContentsMargins(8, 0, 0, 0);
    hbox->addWidget(label);
    hbox->addStretch(10);
    backButton = new QMaemo5BackButton();
    hbox->addWidget(backButton);
    QObject::connect(backButton, SIGNAL(clicked()), q, SLOT(_q_backClicked()));

    q->setFixedHeight(backButton->sizeHint().height());
}

void QMaemo5EditBarPrivate::_q_backClicked()
{
    Q_Q(QMaemo5EditBar);
    q->window()->close();
}

/*!
    \class QMaemo5EditBar
    \since 4.6
    \ingroup qtmaemo5
    \brief The QMaemo5EditBar widget provides a Maemo 5 specific edit toolbar
    showing a label and an arbitrary number of buttons.

    The Maemo 5 edit bar class is a Maemo 5 specific class that is used to
    implement the Hildon edit-toolbar functionality in full screen widgets.

    This widget needs to be shown at the top edge of a full screen widget.
    Clicking the "back" button on the edit bar will call close() on the edit
    bar's window().

    Please see the Maemo 5 Widget, UI-Style and Master-Layout guides for
    hints on when, where and how to best use this button.
*/

/*!
    Constructs an empty edit bar with the given \a parent.
*/
QMaemo5EditBar::QMaemo5EditBar(QWidget *parent)
    : QWidget(parent), d_ptr(new QMaemo5EditBarPrivate)
{
    Q_D(QMaemo5EditBar);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    Constructs an edit bar with the given \a parent widget and editable \a text.
*/
QMaemo5EditBar::QMaemo5EditBar(const QString &text, QWidget *parent)
    : QWidget(parent), d_ptr(new QMaemo5EditBarPrivate)
{
    Q_D(QMaemo5EditBar);
    d_ptr->q_ptr = this;
    d->init();
    setText(text);
}

/*! \internal
*/
QMaemo5EditBar::QMaemo5EditBar(QMaemo5EditBarPrivate &dd, QWidget *parent)
    : QWidget(parent), d_ptr(&dd)
{
    Q_D(QMaemo5EditBar);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    Destroys the edit bar.
*/
QMaemo5EditBar::~QMaemo5EditBar()
{
}

/*! \reimp
*/
void QMaemo5EditBar::paintEvent(QPaintEvent *)
{
    if (qobject_cast<QMaemo5Style *>(style())) {
        QPainter painter(this);
        QStyleOption option;
        option.initFrom(this);
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(QMaemo5Style::PE_Maemo5EditBar),
                               &option, &painter, this);
    }
}

/*!
\property QMaemo5EditBar::text
\brief the text shown on the edit bar

If the edit bar has no text, the text() function will return an empty string.

There is no default text.
*/
void QMaemo5EditBar::setText(const QString &text)
{
    Q_D(QMaemo5EditBar);
    d->label->setText(text);
}

QString QMaemo5EditBar::text() const
{
    Q_D(const QMaemo5EditBar);
    return d->label->text();
}

/*!
    Adds the given \a button to the edit bar.

    \sa removeButton(), buttons()
*/
void QMaemo5EditBar::addButton(QAbstractButton *button)
{
    Q_D(QMaemo5EditBar);
    if (!d->buttons.contains(button)) {
        d->buttons.append(button);
        d->hbox->insertWidget(d->hbox->count() -1, button);
    }
}

/*!
    Removes \a button from the edit bar without deleting it.

    \sa addButton(), buttons()
*/
void QMaemo5EditBar::removeButton(QAbstractButton *button)
{
    Q_D(QMaemo5EditBar);
    if (d->buttons.contains(button)) {
        d->buttons.removeAll(button);
        d->hbox->removeWidget(button);
        button->setParent(0);
    }
}

/*!
    Returns a list of all the buttons that have been added to the edit bar.

    \sa addButton(), removeButton()
*/
QList<QAbstractButton *> QMaemo5EditBar::buttons() const
{
    Q_D(const QMaemo5EditBar);
    return d->buttons;
}

QT_END_NAMESPACE

#include "moc_qmaemo5editbar.cpp"
