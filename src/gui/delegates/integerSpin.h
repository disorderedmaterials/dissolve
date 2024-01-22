// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QModelIndex>
#include <QObject>
#include <QSpinBox>
#include <QtWidgets/QItemDelegate>

class IntegerSpinDelegate : public QItemDelegate
{
    Q_OBJECT

    private:
    // Parameters for QSpinBox
    int min_, max_, step_;

    public:
    IntegerSpinDelegate(QObject *parent = 0, int vmin = -1e6, int vmax = 1e6, int vstep = 1);
    // Reimplemented virtual function to create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // Set the data to appear when the editor is called
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // Extract data from editor, and place back into source model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // Resize editor
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
