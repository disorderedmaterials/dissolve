// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/delegates/comboList.h"
#include "gui/helpers/comboPopulator.h"
#include <QComboBox>
#include <QModelIndex>
#include <QObject>
#include <QtWidgets/QItemDelegate>

// Delegate class, providing list of functional forms / master terms for an intramolecular definition
template <typename T> class IntraFormComboDelegate : public QItemDelegate
{
    private:
    // Associated item list
    ComboListItems *items_;
    // List of master terms
    const T &masterTerms_;

    public:
    IntraFormComboDelegate(QObject *parent, ComboListItems *items, const T &masterTerms);
    virtual ~IntraFormComboDelegate() = default;
    // Reimplemented virtual function to create editing widget
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    // Set the data to appear when the editor is called
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    // Extract data from editor, and place back into source model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    // Resize editor
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

template <typename T>
IntraFormComboDelegate<T>::IntraFormComboDelegate(QObject *parent, ComboListItems *items, const T &masterTerms)
    : QItemDelegate(parent), masterTerms_(masterTerms)
{
    items_ = items;
}

// Create editor
template <typename T>
QWidget *IntraFormComboDelegate<T>::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    // Create editor widget (in this case a combo box) and add the available options
    auto *editor = new QComboBox(parent);

    // Add on standard bond forms first
    items_->restartIterator();
    while (items_->nextItem())
        editor->addItem(items_->currentItemText());

    // Now append any MasterBonds we have
    if (!masterTerms_.empty())
        ComboNameListPopulator(editor, masterTerms_, "@", true);

    return editor;
}

// Set initial value in editor
template <typename T> void IntraFormComboDelegate<T>::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get the current text and search for it in the combo
    QString value = index.model()->data(index, Qt::EditRole).toString();

    for (auto n = 0; n < comboBox->count(); ++n)
    {
        if (comboBox->itemText(n) == value)
        {
            comboBox->setCurrentIndex(n);
            break;
        }
    }
}

// Get value from editing widget, and set back in model
template <typename T>
void IntraFormComboDelegate<T>::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Set the current text in the model
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
template <typename T>
void IntraFormComboDelegate<T>::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                     const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
