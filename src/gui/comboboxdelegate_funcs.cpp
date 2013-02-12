/*
	*** ComboBoxDelegate Functions
	*** src/gui/comboboxdelegate_funcs.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/comboboxdelegate.uih"
#include "base/lineparser.h"
#include "base/messenger.h"

ComboBoxDelegate::ComboBoxDelegate(QObject* parent, const char* itemList) : QItemDelegate(parent)
{
	// Private variables
	LineParser parser;
	parser.getArgsDelim(LineParser::Defaults, itemList);
	for (int n=0; n<parser.nArgs(); ++n)
	{
		Dnchar* s = items_.add();
		s->set(parser.argc(n));
	}
}

// Create editor
QWidget *ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	// Create editor widget (in this case a double spin box) and set some properties
	QComboBox* editor = new QComboBox(parent);
	int maxLength = 0;

	for (Dnchar* s = items_.first(); s != NULL; s = s->next)
	{
		editor->addItem(s->get());
		if (s->length() > maxLength) maxLength = s->length();
	}
	editor->setMinimumContentsLength(maxLength);

	return editor;
}

// Set initial value in editor
void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
	// Get text from ComboBox
	Dnchar text = qPrintable(index.model()->data(index, Qt::EditRole).toString());
	int n;
	for (n = 0; n < items_.nItems(); ++n) if (text == items_.item(n)->get()) break;
	if (n == items_.nItems())
	{
		msg.error("Couldn't find item in '%s' in internal ComboBoxDelegate item list.\n", text.get());
		return;
	}
	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	comboBox->setCurrentIndex(n);
}

// Get value from editing widget, and set back in model
void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}
