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

#ifndef QMAEMOS5STYLE_P_H
#define QMAEMOS5STYLE_P_H

#include <QMap>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef MAEMO_CHANGES
#define MAEMO_CHANGES
#endif

#include "qgtkstyle_p_p.h"

QT_BEGIN_NAMESPACE

class QAbstractScrollArea;
class QTimer;
class QTimeLine;
class ScrollBarFader;

class QMaemo5StylePrivate : public QGtkStylePrivate
{
public:
    static QString systemIconThemeName();
    virtual void initGtkMenu() const;
    virtual void initGtkWidgets() const;
    virtual void applyCustomPaletteHash();
    QPalette gtkWidgetPalette(const QHashableLatin1Literal &gtkWidgetName) const;

    static QString getGtkNameFromQtName(const QWidget *widget);
    static GtkWidget * getGtkButtonForWidget(const QWidget *widget);
    static GtkWidget * addGtkButtonWithName(const QString & widgetName);
    static GtkWidget * addGtkCheckButtonWithName(const QString & widgetName);

    static void setupGtkFileChooser(GtkWidget* gtkFileChooser, QWidget *parent,
            const QString &dir, const QString &filter, QString *selectedFilter,
            QFileDialog::Options options, bool isSaveDialog = false,
            QMap<GtkFileFilter *, QString> *filterMap = 0);

    static QString openFilename(QWidget *parent, const QString &caption, const QString &dir, const QString &filter,
                                QString *selectedFilter, QFileDialog::Options options);
    static QString saveFilename(QWidget *parent, const QString &caption, const QString &dir, const QString &filter,
                                QString *selectedFilter, QFileDialog::Options options);
    static QString openDirectory(QWidget *parent, const QString &caption, const QString &dir,
                                QFileDialog::Options options);
    static QStringList openFilenames(QWidget *parent, const QString &caption, const QString &dir,
                                const QString &filter, QString *selectedFilter, QFileDialog::Options options);

    int getAppMenuMetric( const char *metricName, int defaultValue ) const;
    static QFont getThemeFont(const QString &logicalFontName);

    static QMap<QAbstractScrollArea *, ScrollBarFader *> scrollBarFaders;
    static int scrollBarFadeDelay;
    static int scrollBarFadeDuration;
    static int scrollBarFadeUpdateInterval;

    static GtkWidget *radioButtonLeft;
    static GtkWidget *radioButtonMiddle;
    static GtkWidget *radioButtonRight;

private:
    virtual GtkWidget* getTextColorWidget() const;

};

class ScrollBarFader : public QObject {
    Q_OBJECT
public:
    ScrollBarFader(QAbstractScrollArea *area, int delay, int duration, int update_interval);
    ~ScrollBarFader();

    void show();

    inline qreal currentAlpha() const  { return m_current_alpha; }

private slots:
    void delayTimeout();
    void fade(qreal);

private:
    QAbstractScrollArea *m_area;
    QTimeLine *m_fade_timeline;
    QTimer *m_delay_timer;
    qreal m_current_alpha;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif //QMAEMOS5STYLE_P_H
