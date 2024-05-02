// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/delegates/comboList.h"

/*
 * ComboListItems
 */

ComboListItems::ComboListItems() {}

ComboListItems::~ComboListItems() {}

/*
 * ComboListDelegate
 */

ComboListDelegate::ComboListDelegate(QObject *parent, ComboListItems *items, bool allowNewItems)
    : QItemDelegate(parent), items_(items)
{
    // Private variables
    allowNewItems_ = allowNewItems;
}

ComboListDelegate::~ComboListDelegate()
{
    if (items_)
        delete items_;
}

/*
 * QItemDelegate Virtuals
 */

// Create editor
QWidget *ComboListDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create editor widget (in this case a combo box) and add the available options
    auto *editor = new QComboBox(parent);
    items_->restartIterator();
    while (items_->nextItem())
        editor->addItem(items_->currentItemText());
    editor->setEditable(allowNewItems_);

    // Connect index changed signal of combobox to a local slot, so we can signal to close the editor immediately
    connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(comboIndexChanged(int)));
    return editor;
}

// Set initial value in editor
void ComboListDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();

    // Find this AtomType in the master list
    auto typeIndex = 0;
    items_->restartIterator();
    while (items_->nextItem())
    {
        if (value == items_->currentItemText())
            break;
        ++typeIndex;
    }

    auto *comboBox = static_cast<QComboBox *>(editor);
    comboBox->setCurrentIndex(typeIndex);
}

// Get value from editing widget, and set back in model
void ComboListDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    auto *comboBox = static_cast<QComboBox *>(editor);
    QString value = comboBox->currentText();

    model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void ComboListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

/*
 * Signals / Slots
 */

// Index changed in combo box
void ComboListDelegate::comboIndexChanged(int index)
{
    auto *editor = qobject_cast<QComboBox *>(sender());

    Q_EMIT commitData(editor);
    Q_EMIT closeEditor(editor);
}
