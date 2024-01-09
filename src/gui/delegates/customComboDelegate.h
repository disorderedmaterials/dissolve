// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QComboBox>
#include <QModelIndex>
#include <QObject>
#include <QStyledItemDelegate>

// Custom Combo Delegate
template <class P> class CustomComboDelegate : public QStyledItemDelegate
{
    public:
    // Typedef for function pointers
    typedef std::vector<std::string> (P::*AvailableItemsFunction)(const QModelIndex &index);

    private:
    // Parent object of valid items member function
    P *functionParent_;
    // Function to acquire valid items for the combo box
    AvailableItemsFunction availableItemsFunction_;

    public:
    CustomComboDelegate(P *parent, AvailableItemsFunction availableItemsFunction)
        : QStyledItemDelegate(parent), functionParent_(parent), availableItemsFunction_(availableItemsFunction)
    {
    }

    /*
     * QItemDelegate Reimplementations
     */
    public:
    // Create editor
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        // Create editor widget (in this case a combo box) and add the available options
        QComboBox *editor = new QComboBox(parent);

        // Get the vector of allowable item names
        std::vector<std::string> validNames = (functionParent_->*availableItemsFunction_)(index);

        // Add our text items to the list
        for (auto name : validNames)
            editor->addItem(QString::fromStdString(name));

        return editor;
    }
    // Set initial value in editor
    void setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        // Grab (cast) the QComboBox
        QComboBox *comboBox = static_cast<QComboBox *>(editor);

        // Get the current text and search for it in the combo
        QString value = index.model()->data(index, Qt::EditRole).toString();

        comboBox->setCurrentText(value);
    }
    // Get value from editing widget, and set back in model
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
    {
        // Grab (cast) the QComboBox
        QComboBox *comboBox = static_cast<QComboBox *>(editor);

        // Set the current text in the model
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
    // Update widget geometry
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        editor->setGeometry(option.rect);
    }
};
