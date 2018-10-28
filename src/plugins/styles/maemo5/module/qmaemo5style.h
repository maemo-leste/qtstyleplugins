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

#ifndef QMAEMOS5STYLE_H
#define QMAEMOS5STYLE_H

#include <QtGui/QPalette>
#include <QtGui/QFont>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOption>

#include "qgtkstyle_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPainterPath;
class QMaemo5StylePrivate;
class QAbstractScrollArea;

class Q_GUI_EXPORT QStyleOptionMaemo5ValueButton : public QStyleOptionButton
{
public:
    enum StyleOptionType { Type = SO_CustomBase / 2 };
    enum StyleOptionVersion { Version = 1 };

    enum ButtonStyle {
        ValueBesideText = 0x00,
        ValueUnderText = 0x01,
        Centered = 0x10,
        PickButton = 0x20
    };
    Q_DECLARE_FLAGS(ButtonStyles, ButtonStyle)

    ButtonStyles styles;
    QString value;

    QStyleOptionMaemo5ValueButton() { type = Type; }
    QStyleOptionMaemo5ValueButton(const QStyleOptionMaemo5ValueButton &other) : QStyleOptionButton(Version) { *this = other; type = other.type; }

protected:
    QStyleOptionMaemo5ValueButton(int version);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QStyleOptionMaemo5ValueButton::ButtonStyles)



class Q_GUI_EXPORT QMaemo5Style : public QGtkStyle
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMaemo5Style)

public:
    QMaemo5Style();
    ~QMaemo5Style();

    enum PrimitiveElement {
        PE_Maemo5InformationBox = PE_CustomBase / 2,
        PE_Maemo5AppMenu,
        PE_Maemo5EditBar,
        PE_Maemo5EditBarBackButton
    };

    enum PixelMetric {
        PM_Maemo5AppMenuHorizontalSpacing = PM_CustomBase / 2,
        PM_Maemo5AppMenuVerticalSpacing,
        PM_Maemo5AppMenuContentMargin,
        PM_Maemo5AppMenuLandscapeXOffset,
        PM_Maemo5AppMenuFilterGroupWidth,
        PM_Maemo5AppMenuFilterGroupVerticalSpacing,
        PM_Maemo5EditBarBackButtonWidth,
        PM_Maemo5EditBarBackButtonHeight
    };

    static QFont standardFont(const QString &logicalFontName);
    static QColor standardColor(const QString &logicalColorName);

    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget) const;
    void drawControl(ControlElement control, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget) const;

    int pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option = 0,
                    const QWidget *widget = 0) const;
    int styleHint(StyleHint hint, const QStyleOption *option,
                  const QWidget *widget, QStyleHintReturn *returnData) const;

    QStyle::SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                              const QPoint &pt, const QWidget *w) const;

    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
                         SubControl subControl, const QWidget *widget) const;
    QRect subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *w) const;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const;

    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt = 0,
                           const QWidget *widget = 0) const;

    void polish(QApplication *app);
    void polish(QWidget *widget);
    void unpolish(QWidget *widget);

    void showScrollIndicators(QAbstractScrollArea *);

protected Q_SLOTS:
    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option,
                                     const QWidget *widget = 0) const;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif //QMAEMOS5STYLE_H
