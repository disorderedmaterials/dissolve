/*
	*** IntraFormComboDelegate Functions
	*** src/gui/delegates/intraformcombo_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/delegates/intraformcombo.hui"
#include "gui/helpers/combopopulator.h"
#include "classes/speciesbond.h"
#include "templates/list.h"
#include "templates/variantpointer.h"
#include <QTableWidgetItem>

IntraFormComboDelegate::IntraFormComboDelegate(QObject* parent, int nFunctions, const char** functionTexts, const List<MasterIntra>& masterTerms) : QItemDelegate(parent), masterTerms_(masterTerms)
{
	nFunctions_ = nFunctions;
	functionTexts_ = functionTexts;
}

// Destructor
IntraFormComboDelegate::~IntraFormComboDelegate()
{
}

// Create editor
QWidget* IntraFormComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a combo box) and add the available options
	QComboBox* editor = new QComboBox(parent);

	// Add on standard bond forms first
	ComboPopulator(editor, nFunctions_, functionTexts_);

	// Now append any MasterBonds we have
	if (masterTerms_.nItems() > 0) ComboNameListPopulator<MasterIntra>(editor, masterTerms_, "@", true);

	return editor;
}

// Set initial value in editor
void IntraFormComboDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
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
void IntraFormComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// Grab (cast) the QComboBox
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	// Set the current text in the model
	model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void IntraFormComboDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
