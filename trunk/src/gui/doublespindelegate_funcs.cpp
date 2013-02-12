/*
	*** DoubleSpinDelegate Functions
	*** src/gui/doublespindelegate_funcs.cpp
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

#include "gui/doublespindelegate.uih"

DoubleSpinDelegate::DoubleSpinDelegate(QObject* parent, double vmin, double vmax, double vstep, double nDecimals) : QItemDelegate(parent)
{
	// Private variables
	min_ = vmin;
	max_ = vmax;
	step_ = vstep;
	nDecimals_ = nDecimals;
}

// Create editor
QWidget *DoubleSpinDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	// Create editor widget (in this case a double spin box) and set some properties
	QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
	editor->setMinimum(min_);
	editor->setMaximum(max_);
	editor->setSingleStep(step_);
	editor->setDecimals(nDecimals_);

	return editor;
}

// Set initial value in editor
void DoubleSpinDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
	double value = index.model()->data(index, Qt::EditRole).toDouble();

	QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
	spinBox->setValue(value);
}

// Get value from editing widget, and set back in model
void DoubleSpinDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
	QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
	spinBox->interpretText();
	double value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

// Update widget geometry
void DoubleSpinDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}
