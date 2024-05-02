// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/widgets/exponentialSpin.h"
#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>

class ExponentialSpinDelegate : public QItemDelegate
{
    Q_OBJECT

    private:
    // Parameters for ExponentialSpin
    double min_, max_, step_;
    int nDecimals_;

    public:
    ExponentialSpinDelegate(QObject *parent = 0, double vmin = -1e6, double vmax = 1e6, double vstep = 1.0, int nDecimals = 5);
    // Reimplemented virtual function to create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // Set the data to appear when the editor is called
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // Extract data from editor, and place back into source model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // Resize editor
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
