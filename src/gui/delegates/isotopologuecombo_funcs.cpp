/*
	*** IsotopologueComboDelegate Functions
	*** src/gui/delegates/isotopologuecombo_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/delegates/isotopologuecombo.hui"
#include "gui/helpers/combopopulator.h"
#include "classes/species.h"
#include "templates/list.h"
#include "templates/variantpointer.h"
#include <QTableWidgetItem>

IsotopologueComboDelegate::IsotopologueComboDelegate(QObject* parent) : QItemDelegate(parent)
{
}

// Destructor
IsotopologueComboDelegate::~IsotopologueComboDelegate()
{
}

// Create editor
QWidget* IsotopologueComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a combo box) and add the available options
	QComboBox* editor = new QComboBox(parent);

	// Get the model UserData for the current index - it should be a Species
	Species* sp = (Species*) VariantPointer<Species>(index.data(Qt::UserRole));
	if (sp)
	{
		ComboNameListPopulator<Isotopologue>(editor, sp->isotopologues());
	}
	else Messenger::error("Underlying model did not contain a Species*, so IsotopologueCombo cannot provide options.\n");

	return editor;
}

// Set initial value in editor
void IsotopologueComboDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	// Grab (cast) the QComboBox
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	// Get the current text and search for it in the combo
	QString value = index.model()->data(index, Qt::EditRole).toString();

	for (int n=0; n<comboBox->count(); ++n)
	{
		if (comboBox->itemText(n) == value)
		{
			comboBox->setCurrentIndex(n);
			break;
		}
	}
}

// Get value from editing widget, and set back in model
void IsotopologueComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// Grab (cast) the QComboBox
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	// Set the current text in the model
	model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void IsotopologueComboDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
