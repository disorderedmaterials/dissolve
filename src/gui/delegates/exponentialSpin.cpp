// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/delegates/exponentialSpin.h"

ExponentialSpinDelegate::ExponentialSpinDelegate(QObject *parent, double vmin, double vmax, double vstep, int nDecimals)
    : QItemDelegate(parent)
{
    // Private variables
    min_ = vmin;
    max_ = vmax;
    step_ = vstep;
    nDecimals_ = nDecimals;
}

// Create editor
QWidget *ExponentialSpinDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    // Create editor widget (in this case a double spin box) and set some properties
    auto *editor = new ExponentialSpin(parent);
    editor->setMinimum(min_);
    editor->setMaximum(max_);
    editor->setSingleStep(step_);
    editor->setDecimals(nDecimals_);

    return editor;
}

// Set initial value in editor
void ExponentialSpinDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    double value = index.model()->data(index, Qt::EditRole).toDouble();

    auto *spinBox = static_cast<ExponentialSpin *>(editor);
    spinBox->setValue(value);
    spinBox->selectAll();
}

// Get value from editing widget, and set back in model
void ExponentialSpinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *spinBox = static_cast<ExponentialSpin *>(editor);

    // Update value from current text
    spinBox->updateValueFromText();

    model->setData(index, spinBox->value(), Qt::EditRole);
}

// Update widget geometry
void ExponentialSpinDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
