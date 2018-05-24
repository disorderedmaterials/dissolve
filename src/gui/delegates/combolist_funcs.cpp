/*
	*** ComboList Functions
	*** src/gui/delegates/combolist_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/delegates/combolist.hui"
#include "templates/list.h"
#include "templates/variantpointer.h"

/*
 * ComboListItems
 */

// Constructor
ComboListItems::ComboListItems()
{
}

// Destructor
ComboListItems::~ComboListItems()
{
}

/*
 * ComboListDelegate
 */

ComboListDelegate::ComboListDelegate(QObject* parent, ComboListItems* items, bool allowNewItems) : QItemDelegate(parent), items_(items)
{
	// Private variables
	allowNewItems_ = allowNewItems;
}

// Destructor
ComboListDelegate::~ComboListDelegate()
{
	if (items_) delete items_;
}

// Create editor
QWidget* ComboListDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a combo box) and add the available options
	QComboBox* editor = new QComboBox(parent);
	items_->restartIterator();
	while (items_->nextItem()) editor->addItem(items_->currentItemText());
	editor->setEditable(allowNewItems_);

	return editor;
}

// Set initial value in editor
void ComboListDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	// Find this AtomType in the master list
	int typeIndex = 0;
	items_->restartIterator();
	while (items_->nextItem())
	{
		if (value == items_->currentItemText()) break;
		++typeIndex;
	}

	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	comboBox->setCurrentIndex(typeIndex);
}

// Get value from editing widget, and set back in model
void ComboListDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	QString value = comboBox->currentText();

	model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void ComboListDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
