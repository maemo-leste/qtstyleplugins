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

#include <QMaemo5InformationBox>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLayout>
#include <QtMaemo5/QMaemo5Style>
#include <QtWidgets/QStyleOption>
#include <QtGui/QMouseEvent>
#include <QPainter>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMaemo5InformationBox
    \since 4.6
    \brief The QMaemo5InformationBox class provides Hildon Banners and Notes
    for informing the user.

    \ingroup standard-dialogs
    \ingroup qtmaemo5

    The Maemo 5 information box class is a Maemo 5 specific class that is used
    to display an arbitrary widget with or without a timeout to alert
    the user to a situation.

    Information boxes that have a timeout will show as Hildon Banners, which
    will be hidden after the specified time.  In the case where \c NoTimeout
    is specified for the timeout value, the information box will show up as a
    Hildon Note, which has to be acknowledged by the user with a tap anywhere
    on the screen.

    Two APIs for using QMaemo5InformationBox are provided: a property-based API
    and a static function.  Calling the static function is the simpler approach,
    but it is less flexible than using the property-based API, since the widget
    is hardcoded to a QLabel.

    Using the static function API is recommended only if textual information
    is to be displayed.

    \section1 The Property-based API

    To use the property-based API, construct an instance of
    QMaemo5InformationBox, create the information widget, set the desired
    timeout, and call exec() (or show() for simple Banners) to show the
    message.

    The clicked() signal will be emitted if the user clicks on the
    QMaemo5InformationBox, regardless of whether the information is shown with
    or without a timeout.

    Note that, using the static function for some plain text strings
    containing XML meta-characters, the auto-text \l{Qt::mightBeRichText()}
    {rich text detection test} may fail causing your plain text string to be
    interpreted incorrectly as rich text.  In these rare cases, use
    Qt::convertFromPlainText() to convert your plain text string to a
    visually equivalent rich text string, or use the property-based API and
    set your custom QLabel with setWidget().

    The \c DefaultTimeout (3 seconds) is the timeout recommended by the
    Hildon style guide. The maximum timeout is 10 seconds.

    Please see the Maemo 5 Widget, UI-Style and Master-Layout guides for
    hints on when, where and how to best use this information box.
*/

/*!
    \fn void QMaemo5InformationBox::clicked()

    This signal is emitted whenever the QMaemo5InformationBox is clicked.
*/

class QMaemo5InformationBoxPrivate
{
    Q_DECLARE_PUBLIC(QMaemo5InformationBox)

public:
    QMaemo5InformationBoxPrivate()
        : m_timeout(QMaemo5InformationBox::DefaultTimeout),
          m_timer_id(0), m_layout(0), m_widget(0), q_ptr(0)
    { }

    void enforceInformationType();

    int m_timeout;
    int m_timer_id;
    QBoxLayout *m_layout;
    QWidget *m_widget;
    QMaemo5InformationBox *q_ptr;
};

/*!
    Constructs an information box with no text, a default timeout
    (\c DefaultTimeout) and the given \a parent widget.
*/
QMaemo5InformationBox::QMaemo5InformationBox(QWidget *parent)
    : QDialog(parent, Qt::CustomizeWindowHint), d_ptr(new QMaemo5InformationBoxPrivate())
{
    Q_D(QMaemo5InformationBox);
    d_ptr->q_ptr = this;

    setAttribute(Qt::WA_X11NetWmWindowTypeNotification);
    setAttribute(Qt::WA_X11DoNotAcceptFocus);
    setFocusPolicy(Qt::NoFocus);
    d->m_layout = new QHBoxLayout(this);
    d->m_layout->setContentsMargins(8, 0, 8, 0); // 8 == HILDON_MARGIN_DEFAULT
    setFixedWidth(QApplication::desktop()->screenGeometry().width());
}

/*!
    Destroys the information box.
*/
QMaemo5InformationBox::~QMaemo5InformationBox()
{ }

/*! \internal
 */
void QMaemo5InformationBoxPrivate::enforceInformationType()
{
    Q_Q(QMaemo5InformationBox);

    const char *type = (m_timeout != QMaemo5InformationBox::NoTimeout)
                        ? "_HILDON_NOTIFICATION_TYPE_BANNER"
                        : "_HILDON_NOTIFICATION_TYPE_INFO";
    Atom atom = XInternAtom(QX11Info::display(), "_HILDON_NOTIFICATION_TYPE", False);

    XChangeProperty(QX11Info::display(), q->winId(), atom, XA_STRING, 8,
                    PropModeReplace, (unsigned char *) type, qstrlen(type));
}

/*!
  \property QMaemo5InformationBox::timeout
  \brief the timeout after which the informaton box should automatically be
  hidden.

  Setting this value while the box is already visible will reset its timer.

  The timeout value can be specified as an arbitrary millisecond value,
  although it is recommended to use the predefined values \c NoTimeout (for
  Hildon Notes) and \c DefaultTimeout (for style guide compliant Banners).

  The default value of this property is \c DefaultTimeout (3 seconds).
  The maximum timeout is 10 seconds.
*/

int QMaemo5InformationBox::timeout() const
{
    Q_D(const QMaemo5InformationBox);
    return d->m_timeout;
}

void QMaemo5InformationBox::setTimeout(int ms)
{
    Q_D(QMaemo5InformationBox);
    d->m_timeout = qBound(0, ms, 10000);

    // restart the timer
    if (d->m_timeout != QMaemo5InformationBox::NoTimeout
            && d->m_timer_id) {
        killTimer(d->m_timer_id);
        d->m_timer_id = startTimer(d->m_timeout);
    }
}

/*!
    Returns the central widget of the information box.
*/
QWidget *QMaemo5InformationBox::widget() const
{
    Q_D(const QMaemo5InformationBox);
    return d->m_widget;
}

/*!
    Sets the central widget of the information box to \a widget.

    QMaemo5InformationBox will take ownership of \a widget by reparenting
    it.  Any previously set widget() will be destroyed.
*/
void QMaemo5InformationBox::setWidget(QWidget *widget)
{
    Q_D(QMaemo5InformationBox);
    delete d->m_widget;
    d->m_widget = widget;
    if (widget) {
        d->m_layout->addWidget(widget);
        widget->show();
    }
}

/*!
    Opens an information message box with the specified \a message and \a
    timeout.

    If the \a timeout parameter equals \c NoTimeout, the information box will
    show as a Hildon Note and this function will block until the user clicks
    anywhere on the screen.  Otherwise, the information box show as a Hildon
    Banner, which will be hidden after \a timeout milliseconds.  The function will
    return to the caller immediately in this case.

    If there is no \a timeout specified and \a parent is 0, the information
    box is an \l{Qt::ApplicationModal}{application modal} dialog box.
    If \a parent is a widget, the information box is \l{Qt::WindowModal}
    {window modal} relative to the \a parent.
*/
void QMaemo5InformationBox::information(QWidget *parent, const QString &message, int timeout)
{
    QMaemo5InformationBox *box = new QMaemo5InformationBox(parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->setTimeout(timeout);
    QLabel *label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::NoTextInteraction);
    label->setText(message);
    box->setWidget(label);

    if (timeout > 0)
        box->show();
    else
        box->exec();
}

/*! \reimp
*/
void QMaemo5InformationBox::showEvent(QShowEvent *)
{
    Q_D(QMaemo5InformationBox);

    d->enforceInformationType();
    if (d->m_timeout != QMaemo5InformationBox::NoTimeout)
        d->m_timer_id = startTimer(d->m_timeout);
}

/*! \reimp
*/
void QMaemo5InformationBox::timerEvent(QTimerEvent *te)
{
    Q_D(QMaemo5InformationBox);

    if (te->timerId() == d->m_timer_id) {
        killTimer(d->m_timer_id);
        close();
    }
}

/*! \reimp
*/
void QMaemo5InformationBox::mousePressEvent(QMouseEvent *me)
{
    if (me->button() == Qt::LeftButton) {
        close();
        emit clicked();
    }
}

/*! \reimp
*/
void QMaemo5InformationBox::paintEvent(QPaintEvent *)
{
        QPainter painter(this);
        QStyleOption option;
        option.initFrom(this);
        style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(QMaemo5Style::PE_Maemo5InformationBox),
                               &option, &painter, this);
}

QT_END_NAMESPACE
