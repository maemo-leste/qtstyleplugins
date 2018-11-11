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
#include <qmaemo5valuebutton.h>

#include <QPainter>
#include <QButtonGroup>
#include <QStyle>
#include <QStylePainter>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QApplication>

#include "qmaemo5style.h"

#include <qmaemo5abstractpickselector.h>
#include <qmaemo5valuebutton_p.h>

#include <QtDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QMaemo5ValueButton
    \brief The QMaemo5ValueButton widget provides a Maemo 5 specific button with two labels.
    \since 4.6
    \ingroup basicwidgets
    \ingroup qtmaemo5

    The Maemo 5 value button class is a Maemo 5 specific class that is mainly
    used to implement so called "picker buttons" on this platform.

    It has all the properties of a normal QPushButton, but comes with an
    additional label that can be set with setValueText().  This value label
    can be either positioned to the right or below the normal text() using
    setValueLayout().

    If the button is to be used as a picker button then the setPickSelector()
    interface should be used.

    Depending on the current Maemo theme, a QMaemo5ValueButton that has a
    pickSelector() set on it, may render a differently to one without a
    selector.

    Please see the Maemo 5 Widget, UI-Style and Master-Layout guides for
    hints on when, where and how to best use this button.

    \sa QPushButton, QMaemo5AbstractPickSelector
*/


/*! \internal
 */
void QMaemo5ValueButtonPrivate::init()
{
    Q_Q(QMaemo5ValueButton);
    QObject::connect(q, SIGNAL(clicked()), q, SLOT(_q_clicked()));
}

/*! \internal
    used to popup the pick selector, if one was set on this button
 */
void QMaemo5ValueButtonPrivate::_q_clicked()
{
    Q_Q(QMaemo5ValueButton);

    if (!pickSelector)
        return;
    QWidget *w = pickSelector->widget(q);
    if (!w) {
        qWarning("QMaemo5ValueButton: pickSelector()->widget() returned 0.");
    } else if (!w->isWindow()) {
        qWarning("QMaemo5ValueButton: pickSelector()->widget() is not a top-level window.");
        delete w;
    } else {
        w->show();
    }
}

/*! \internal
    callback from the pick selector
 */
void QMaemo5ValueButtonPrivate::_q_selected(const QString &value)
{
    Q_Q(QMaemo5ValueButton);
    q->setValueText(value);
}

/*!
    Constructs a value button with no text and the given \a parent.
*/
QMaemo5ValueButton::QMaemo5ValueButton(QWidget *parent)
    : QPushButton(parent), d_ptr(new QMaemo5ValueButtonPrivate)
{
    Q_D(QMaemo5ValueButton);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    Constructs a value button with the given \a parent containing the \a text
    specified.
*/
QMaemo5ValueButton::QMaemo5ValueButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), d_ptr(new QMaemo5ValueButtonPrivate)
{
    Q_D(QMaemo5ValueButton);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    Constructs a value button with an \a icon and \a text, and the given \a parent.

    Note that you can also pass a QPixmap object as an icon (thanks to
    the implicit type conversion provided by C++).

*/
QMaemo5ValueButton::QMaemo5ValueButton(const QIcon& icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent), d_ptr(new QMaemo5ValueButtonPrivate)
{
    Q_D(QMaemo5ValueButton);
    d_ptr->q_ptr = this;
    d->init();
}

/*! \internal
 */
QMaemo5ValueButton::QMaemo5ValueButton(QMaemo5ValueButtonPrivate &dd, QWidget *parent)
    : QPushButton(parent), d_ptr(&dd)
{
    Q_D(QMaemo5ValueButton);
    d_ptr->q_ptr = this;
    d->init();
}

/*!
    Destroys the value button.
*/
QMaemo5ValueButton::~QMaemo5ValueButton()
{
}

/*!
    \property QMaemo5ValueButton::valueLayout
    \brief the layout of the value text relative to the normal text

    The default is QMaemo5ValueButton::ValueUnderText.

    Please see the Maemo 5 Widget, UI-Style and Master-Layout guides for more information.

    \sa valueText, setPickSelector()
*/
QMaemo5ValueButton::ValueLayout QMaemo5ValueButton::valueLayout() const
{
    Q_D(const QMaemo5ValueButton);
    return d->valueLayout;
}

void QMaemo5ValueButton::setValueLayout(ValueLayout layout)
{
    Q_D(QMaemo5ValueButton);

    if (layout != d->valueLayout) {
        d->valueLayout = layout;
        d->sizeHint = QSize();
        update();
        updateGeometry();
    }
}

/*!
    \property QMaemo5ValueButton::valueText
    \brief the value text shown on the button

    If the button has no value text, the valueText() function will return an
    empty string.

    By default, no text is set.

    \sa valueLayout, setPickSelector()
*/
QString QMaemo5ValueButton::valueText() const
{
    Q_D(const QMaemo5ValueButton);
    return d->valueText;
}

void QMaemo5ValueButton::setValueText(const QString &text)
{
    Q_D(QMaemo5ValueButton);

    if (text != d->valueText) {
        d->valueText = text;
        d->sizeHint = QSize();
        update();
        updateGeometry();
    }
}

/*!
    Returns the pick selector set on this button.

    If there is no pick selector set, this function will return 0.

    By default, no picker is set.

    \sa valueLayout, setPickSelector()
*/
QMaemo5AbstractPickSelector *QMaemo5ValueButton::pickSelector() const
{
    Q_D(const QMaemo5ValueButton);
    return d->pickSelector;
}

/*!
    Sets the pick selector for this button to the \a picker specified.

    If the widget already has a pick selector, it is deleted.
    QMaemo5ValueButton takes ownership of \a picker.

    Call this function with \a picker equal to 0 to unset (and delete) any
    previously set picker.

    \sa pickSelector()
*/
void QMaemo5ValueButton::setPickSelector(QMaemo5AbstractPickSelector *picker)
{
    Q_D(QMaemo5ValueButton);

    if (picker != d->pickSelector) {
        if (d->pickSelector)
            delete d->pickSelector;
        d->pickSelector = picker;
        if (picker) {
            picker->setParent(this);
            connect(picker, SIGNAL(selected(QString)), this, SLOT(_q_selected(QString)));
            setValueText(picker->currentValueText());
        }
        update();
    }
}

/*!
    \reimp
*/
QSize QMaemo5ValueButton::sizeHint() const
{
    Q_D(const QMaemo5ValueButton);
    if (d->sizeHint.isValid())
        return d->sizeHint;
    ensurePolished();

    int w = 0;
    int h = 0;
    int spacing = 8; // standard Hildon spacing

    QStyleOptionButton option;
    initStyleOption(&option);

    QSize iconSize;
    if (!icon().isNull())
        iconSize = option.iconSize;
    d->textSize = fontMetrics().size(Qt::TextSingleLine, text());
    if (d->valueLayout == ValueBesideText) {
        d->valueSize = fontMetrics().size(Qt::TextSingleLine, d->valueText);
    } else {
        QFont f = QMaemo5Style::standardFont(QLatin1String("SmallSystemFont"));
        d->valueSize = QFontMetrics(f).size(Qt::TextSingleLine, d->valueText);
    }

    switch (d->valueLayout) {
    case ValueBesideText:
        w = iconSize.width() + spacing + d->textSize.width() + spacing + d->valueSize.width();
        h = qMax(iconSize.height(), d->textSize.height());
        break;

    case ValueUnderText:
    case ValueUnderTextCentered:
        w = iconSize.width() + spacing + qMax(d->textSize.width(), d->valueSize.width());
        h = qMax(iconSize.height(), d->textSize.height() + spacing + d->valueSize.height());
        break;
    }

    d->sizeHint = style()->sizeFromContents(QStyle::CT_PushButton, &option, QSize(w, h), this).
                  expandedTo(QApplication::globalStrut()).expandedTo(QPushButton::sizeHint());

    return d->sizeHint;
}

/*!
    \reimp
*/
void QMaemo5ValueButton::paintEvent(QPaintEvent *)
{
    Q_D(QMaemo5ValueButton);

    if (!d->sizeHint.isValid())
        sizeHint();

    QStylePainter p(this);
    QStyleOptionButton button;
    initStyleOption(&button);
    QStyleOptionMaemo5ValueButton option;
    initStyleOption(&option);
    option.value = valueText();
    option.styles = (d->valueLayout == ValueBesideText) ? QStyleOptionMaemo5ValueButton::ValueBesideText
                                                        : QStyleOptionMaemo5ValueButton::ValueUnderText;
    if (d->valueLayout == ValueUnderTextCentered)
        option.styles |= QStyleOptionMaemo5ValueButton::Centered;
    if (d->pickSelector)
        option.styles |= QStyleOptionMaemo5ValueButton::PickButton;

    p.drawControl(QStyle::CE_PushButtonBevel, button);
    option.rect = style()->subElementRect(QStyle::SE_PushButtonContents, &button, this);
    p.drawControl(QStyle::CE_PushButtonLabel, option);
}

/*!
    \enum QMaemo5ValueButton::ValueLayout

    This enum describes the positions in which the value label can appear inside the button.

    \value ValueBesideText The value is displayed to the left or to the right of the button text (depending on the text direction).

    \value ValueUnderText The value is displayed underneath the button text in a small font.

    \value ValueUnderTextCentered The value and the button text are centered. The Value is displayed underneath the button text.

    \sa valueLayout(), setValueLayout()
*/

QT_END_NAMESPACE

#include "moc_qmaemo5valuebutton.cpp"
