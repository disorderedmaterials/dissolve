// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/speciesbond.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/intraformcombo.hui"
#include "gui/helpers/combopopulator.h"
#include "templates/list.h"
#include "templates/variantpointer.h"

IntraFormComboDelegate::IntraFormComboDelegate(QObject *parent, ComboListItems *items,
                                               const std::list<MasterIntra> &masterTerms)
    : QItemDelegate(parent), masterTerms_(masterTerms)
{
    items_ = items;
}

IntraFormComboDelegate::~IntraFormComboDelegate() {}

// Create editor
QWidget *IntraFormComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
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
        ComboNameListPopulator<MasterIntra>(editor, masterTerms_, "@", true);

    return editor;
}

// Set initial value in editor
void IntraFormComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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
void IntraFormComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Set the current text in the model
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void IntraFormComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                  const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
