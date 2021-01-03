// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/delegates/integerspin.hui"
#include <stdio.h>

IntegerSpinDelegate::IntegerSpinDelegate(QObject *parent, int vmin, int vmax, int vstep) : QItemDelegate(parent)
{
    // Private variables
    min_ = vmin;
    max_ = vmax;
    step_ = vstep;
}

// Create editor
QWidget *IntegerSpinDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create editor widget (in this case a QSpinBox) and set some properties
    auto *editor = new QSpinBox(parent);
    editor->setMinimum(min_);
    editor->setMaximum(max_);
    editor->setSingleStep(step_);

    return editor;
}

// Set initial value in editor
void IntegerSpinDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto value = index.model()->data(index, Qt::EditRole).toInt();

    auto *spinBox = static_cast<QSpinBox *>(editor);
    spinBox->setValue(value);
}

// Get value from editing widget, and set back in model
void IntegerSpinDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *spinBox = static_cast<QSpinBox *>(editor);

    // Make sure the value in the spinBox has been updated from the current text
    spinBox->interpretText();
    auto value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void IntegerSpinDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
