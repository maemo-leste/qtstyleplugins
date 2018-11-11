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

#include <QDialog>
#include <QtWidgets/QLayout>
#include <QListView>
#include <QHeaderView>
#include <QAbstractButton>
#include <QDialogButtonBox>

#include <qmaemo5listpickselector.h>
#include <qmaemo5listpickselector_p.h>


QT_BEGIN_NAMESPACE


/*! 
    \class QMaemo5ListPickSelector
    \since 4.6
    \ingroup qtmaemo5
    \brief The QMaemo5ListPickSelector widget displays a list of items.

    The QMaemo5ListPickSelector is a selector that displays the contents of a QAbstractItemModel
    and allows the user to select one item from it.

    This object should be used together with the QMaemo5ValueButton.

    \sa QAbstractItemModel, QMaemo5ValueButton
*/

/*!
    Constructs a new list pick selector with the given \a parent object.
*/
QMaemo5ListPickSelector::QMaemo5ListPickSelector(QObject *parent)
    : QMaemo5AbstractPickSelector(*new QMaemo5ListPickSelectorPrivate, parent)
{
    Q_D(QMaemo5ListPickSelector);
    d->init();
}

/*! \internal
*/
QMaemo5ListPickSelector::QMaemo5ListPickSelector(QMaemo5ListPickSelectorPrivate &dd, QObject *parent)
    : QMaemo5AbstractPickSelector(dd, parent)
{
    Q_D(QMaemo5ListPickSelector);
    d->init();
}

/*!
   Destroys the pick selector and the view (if set).
   It will not destroy the model.
*/
QMaemo5ListPickSelector::~QMaemo5ListPickSelector()
{
}

QMaemo5ListPickSelectorPrivate::QMaemo5ListPickSelectorPrivate()
    : model(0)
    , view(0)
    , modelColumn(0)
    , currentIndex(-1)
{
}

QMaemo5ListPickSelectorPrivate::~QMaemo5ListPickSelectorPrivate()
{
    if (view)
        delete view;
}

void QMaemo5ListPickSelectorPrivate::emitSelected()
{
    Q_Q(QMaemo5ListPickSelector);
    emit q->selected(q->currentValueText());
}

/*  The dialog used to display the list (with the previously set model) and
    the buttons.
    Actually the real construction is done by QMaemo5ListPickSelectorPrivate.
*/
class ListPickerDialog : public QDialog {
    Q_OBJECT
public:
    ListPickerDialog(QMaemo5ListPickSelectorPrivate *picker, QWidget *parent)
        : QDialog(parent), listPickSelectorPrivate(picker), itemView(0)
    { }

    ~ListPickerDialog()
    {
        // give back the view
        if (listPickSelectorPrivate->view)
            listPickSelectorPrivate->view->setParent(0); // so that we don't get deleted automatically
    }

public Q_SLOTS:
    void accept(const QModelIndex& index)
    {
        if ((index.flags() & Qt::ItemIsEnabled)
                && (index.flags() & Qt::ItemIsSelectable)) {

            listPickSelectorPrivate->currentIndex = index.row();
            listPickSelectorPrivate->emitSelected();

            QDialog::accept();
        }
    }

public:
    QMaemo5ListPickSelectorPrivate *listPickSelectorPrivate;
    QAbstractItemView *itemView;
};

/*!
    \reimp
*/
QWidget *QMaemo5ListPickSelector::widget(QWidget *parent)
{
    Q_D(QMaemo5ListPickSelector);

    ListPickerDialog *td = new ListPickerDialog(d, parent);
    if (QAbstractButton *pb = qobject_cast<QAbstractButton *>(parent))
        td->setWindowTitle(pb->text());
    td->setAttribute(Qt::WA_DeleteOnClose);
    QHBoxLayout *layout = new QHBoxLayout(td);
    layout->setContentsMargins(16, 0, 16, 8);

    QAbstractItemView *view = d->view;
    if (!view) {
        QListView *listView = new QListView();
        listView->setModel(d->model);
        listView->setModelColumn(d->modelColumn);

        if (listView->sizeHintForRow(0)>0)
            listView->setMinimumHeight(listView->sizeHintForRow(0) * 5);

        view = listView;

    } else {
        view->setModel(d->model);
    }

    layout->addWidget(view);

    if (d->model) {
        QModelIndex index = d->model->index(d->currentIndex, d->modelColumn);
        view->selectionModel()->select(index,
                QItemSelectionModel::Select | QItemSelectionModel::Rows);

        view->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
    td->itemView = view;
    connect(td->itemView, SIGNAL(activated(const QModelIndex&)), td, SLOT(accept(const QModelIndex&)) );

    return td;
}



/*!
    \property QMaemo5ListPickSelector::model
    \brief the model used in the list picker.

    By default, Maemo lists have centered text so it makes sense to call
    setTextAlignment(Qt::AlignCenter) on all items in the model in order
    to comply with the Maemo 5 style guides.

    Either list or table models can be used. Table models may require the
    \l modelColumn property to be set to a non-default value, depending
    on their contents.

    The use of tree models with a pick selector is untested. In addition,
    changing the model while the list dialog is displayed may lead
    to undefined behavior.

    \note The list pick selector will not take ownership of the model.
*/
QAbstractItemModel *QMaemo5ListPickSelector::model() const
{
    Q_D(const QMaemo5ListPickSelector);
    return d->model;
}

void QMaemo5ListPickSelector::setModel(QAbstractItemModel *model)
{
    Q_D(QMaemo5ListPickSelector);

    if (model == d->model)
        return;

    if (d->model)
        disconnect(d->model, SIGNAL(destroyed()), this, SLOT(_q_modelDestroyed()));

    d->model = model;

    if (d->model) {
        if (d->model->rowCount() < d->currentIndex && d->currentIndex >= 0)
            d->currentIndex = d->model->rowCount();

        connect(d->model, SIGNAL(destroyed()), this, SLOT(_q_modelDestroyed()));

    } else {
        d->currentIndex = -1;
    }

    emit selected(currentValueText());
}

/*!
    Returns the list view used for the list pick selector pop-up.
*/
QAbstractItemView *QMaemo5ListPickSelector::view() const
{
    Q_D(const QMaemo5ListPickSelector);
    return d->view;
}

/*!
  Sets the view to be used in the list pick selector pop-up to the given
  \a itemView. The list picker takes ownership of the view.

  \note If you want to use the convenience views (like QListWidget,
  QTableWidget or QTreeWidget), make sure you call setModel() on the
  list picker with the convenience widget's model before calling this
  function.
*/
void QMaemo5ListPickSelector::setView(QAbstractItemView *itemView)
{
    Q_D(QMaemo5ListPickSelector);

    if (itemView == d->view)
        return;

    if (d->view)
        disconnect(d->view, SIGNAL(destroyed()), this, SLOT(_q_viewDestroyed()));

    d->view = itemView;

    if (d->view) {
        connect(d->view, SIGNAL(destroyed()), this, SLOT(_q_viewDestroyed()));
        d->view->setParent(0); // so that we don't get deleted automatically
    }
}

/*! \internal
    Deletes the internal references when the model is destroyed.
    */
void QMaemo5ListPickSelectorPrivate::_q_modelDestroyed()
{
    model = 0;
    currentIndex = -1;
}

/*! \internal
    Deletes the internal references when the view is destroyed.
    */
void QMaemo5ListPickSelectorPrivate::_q_viewDestroyed()
{
    view = 0;
}

/*!
    \property QMaemo5ListPickSelector::modelColumn
    \brief the column in the model that is used for the picker results.

    This property determines the column in a table model used for
    determining the picker result.

    By default, this property has a value of 0.
*/
int QMaemo5ListPickSelector::modelColumn() const
{
    Q_D(const QMaemo5ListPickSelector);
    return d->modelColumn;
}

void QMaemo5ListPickSelector::setModelColumn(int column)
{
    Q_D(QMaemo5ListPickSelector);
    d->modelColumn = column;

    // update the displayed value
    emit selected(currentValueText());
}

/*!
    \property QMaemo5ListPickSelector::currentIndex
    \brief the index of the current item in the list picker.

    The current index can change when setting a new model with a lower row
    count than the previous one.

    By default, for an empty list picker or a list picker in which no current
    item is set, this property has a value of -1.
*/
int QMaemo5ListPickSelector::currentIndex() const
{
    Q_D(const QMaemo5ListPickSelector);
    return d->currentIndex;
}

void QMaemo5ListPickSelector::setCurrentIndex(int index)
{
    Q_D(QMaemo5ListPickSelector);

    int oldIndex = d->currentIndex;
    if (!d->model)
        d->currentIndex = -1;
    else if (index < 0 || index > d->model->rowCount())
        d->currentIndex = 0;
    else
        d->currentIndex = index;

    if (oldIndex != d->currentIndex)
        emit selected(currentValueText());
}

/*! \reimp
*/
QString QMaemo5ListPickSelector::currentValueText() const
{
    Q_D(const QMaemo5ListPickSelector);
    if (d->model && d->currentIndex >= 0)
        return d->model->data(d->model->index(d->currentIndex, d->modelColumn)).toString();
    return QString();
}

QT_END_NAMESPACE

#include "moc_qmaemo5listpickselector.cpp"
#include "qmaemo5listpickselector.moc"
