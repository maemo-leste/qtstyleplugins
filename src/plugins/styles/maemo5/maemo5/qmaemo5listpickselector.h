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

#ifndef QMAEMO5LISTPICKSELECTOR_H
#define QMAEMO5LISTPICKSELECTOR_H

#include <QtMaemo5/qmaemo5abstractpickselector.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


QT_MODULE(Maemo5)


class QAbstractItemModel;
class QAbstractItemView;
class QMaemo5ListPickSelectorPrivate;

class Q_MAEMO5_EXPORT QMaemo5ListPickSelector : public QMaemo5AbstractPickSelector
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model WRITE setModel)
    Q_PROPERTY(int modelColumn READ modelColumn WRITE setModelColumn)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)

public:
    explicit QMaemo5ListPickSelector(QObject *parent = 0);
    ~QMaemo5ListPickSelector();

    QWidget *widget(QWidget *parent);

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *model);

    QAbstractItemView *view() const;
    void setView(QAbstractItemView *itemView);

    int modelColumn() const;
    void setModelColumn(int column);

    QString currentValueText() const;

    int currentIndex() const;

public Q_SLOTS:
    void setCurrentIndex(int index);

protected:
    QMaemo5ListPickSelector(QMaemo5ListPickSelectorPrivate &dd, QObject *parent = 0);

private:
    Q_DISABLE_COPY(QMaemo5ListPickSelector)
    Q_DECLARE_PRIVATE(QMaemo5ListPickSelector)

    Q_PRIVATE_SLOT(d_func(), void _q_modelDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_viewDestroyed())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMAEMO5LISTPICKSELECTOR_H
