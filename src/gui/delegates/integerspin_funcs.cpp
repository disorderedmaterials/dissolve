/*
	*** IntegerSpin Delegate Functions
	*** src/gui/delegates/integerspin_funcs.cpp
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

#include "gui/delegates/integerspin.hui"
#include <stdio.h>

IntegerSpinDelegate::IntegerSpinDelegate(QObject *parent, int vmin, int vmax, int vstep) : QItemDelegate(parent)
{
	// Private variables
	min_ = vmin;
	max_ = vmax;
	step_ = vstep;
}

// Create editor
QWidget* IntegerSpinDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a QSpinBox) and set some properties
	QSpinBox* editor = new QSpinBox(parent);
	editor->setMinimum(min_);
	editor->setMaximum(max_);
	editor->setSingleStep(step_);

	return editor;
}

// Set initial value in editor
void IntegerSpinDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

// Get value from editing widget, and set back in model
void IntegerSpinDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QSpinBox* spinBox = static_cast<QSpinBox*>(editor);

	// Make sure the value in the spinBox has been updated from the current text
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void IntegerSpinDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
