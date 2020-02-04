/*
	*** ExponentialSpin Delegate Functions
	*** src/gui/delegates/exponentialspin_funcs.cpp
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

#include "gui/delegates/exponentialspin.hui"
#include <stdio.h>

ExponentialSpinDelegate::ExponentialSpinDelegate(QObject *parent, double vmin, double vmax, double vstep, double nDecimals) : QItemDelegate(parent)
{
	// Private variables
	min_ = vmin;
	max_ = vmax;
	step_ = vstep;
	nDecimals_ = nDecimals;
}

// Create editor
QWidget* ExponentialSpinDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	// Create editor widget (in this case a double spin box) and set some properties
	ExponentialSpin* editor = new ExponentialSpin(parent);
	editor->setMinimum(min_);
	editor->setMaximum(max_);
	editor->setSingleStep(step_);
	editor->setDecimals(nDecimals_);

	return editor;
}

// Set initial value in editor
void ExponentialSpinDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	double value = index.model()->data(index, Qt::EditRole).toDouble();

	ExponentialSpin* spinBox = static_cast<ExponentialSpin*>(editor);
	spinBox->setValue(value);
	spinBox->selectAll();
}

// Get value from editing widget, and set back in model
void ExponentialSpinDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	ExponentialSpin* spinBox = static_cast<ExponentialSpin*>(editor);

	// Make sure the value in the spinBox has been updated from the current text
	spinBox->updateValueFromText();
	double value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void ExponentialSpinDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
