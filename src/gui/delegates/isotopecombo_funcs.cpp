/*
	*** IsotopeCombo Functions
	*** src/gui/delegates/isotopecombo_funcs.cpp
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

#include "gui/delegates/isotopecombo.hui"
#include "data/isotopes.h"
#include "templates/list.h"
#include "templates/variantpointer.h"

IsotopeComboDelegate::IsotopeComboDelegate(QObject* parent) : QItemDelegate(parent)
{
}

// Destructor
IsotopeComboDelegate::~IsotopeComboDelegate()
{
}

// Create editor
QWidget* IsotopeComboDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a combo box) and add the available options
	QComboBox* editor = new QComboBox(parent);

	// Get the data from the model index - it should be an Isotope*
	Isotope* isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
	if (isotope)
	{
		// Populate combo with all possible Isotopes for this Element
		const Element& element = isotope->element();
		ListIterator<Isotope> isotopeIterator(Isotopes::isotopes(element.Z()));
		while (Isotope* tope = isotopeIterator.iterate()) editor->addItem(textForIsotope(tope));
	}
	else Messenger::error("IsotopeComboDelegate::createEditor() - Did not find an Isotope* in the associated QModelIndex.\n");

	return editor;
}

// Set initial value in editor
void IsotopeComboDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
  {
	// Grab (cast) the QComboBox
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	// Get the data from the model index - it should be an Isotope*
	Isotope* isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
	if (isotope)
	{
		comboBox->setCurrentIndex(isotope->index());
	}
	else Messenger::error("IsotopeComboDelegate::createEditor() - Did not find an Isotope* in the associated QModelIndex.\n");
// 	QString value = index.model()->data(index, Qt::EditRole).toString();
}

// Get value from editing widget, and set back in model
void IsotopeComboDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	// Grab (cast) the QComboBox
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	// Get existing Isotope
	Isotope* isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
	if (isotope)
	{
		// Get parent Element, and find index of new Isotope
		const Element& element = isotope->element();
		isotope = Isotopes::isotopeAtIndex(element.Z(), comboBox->currentIndex());

		// Set the Isotope pointer in the model
		model->setData(index, VariantPointer<Isotope>(isotope), Qt::UserRole);
		model->setData(index, comboBox->currentText(), Qt::EditRole);
	}
}

// Update widget geometry
void IsotopeComboDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

/*
 * Helper Functions
 */

// Return text for Isotope specified
QString IsotopeComboDelegate::textForIsotope(Isotope* isotope)
{
	if (!isotope) return "NULL";

	if (isotope->A() == 0) return QString("Natural (bc = %1)").arg(isotope->boundCoherent());
	else return QString("%1 (bc = %2)").arg(isotope->A()).arg(isotope->boundCoherent());
}
