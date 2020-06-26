/*
    *** Custom Combo Delegate
    *** src/gui/delegates/customcombodelegate.hui
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <QComboBox>
#include <QModelIndex>
#include <QObject>
#include <QStyledItemDelegate>

// Forward Declarations
/* none */

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
