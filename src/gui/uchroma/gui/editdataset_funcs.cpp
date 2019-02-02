/*
	*** Edit DataSet Dialog
	*** src/gui/uchroma/gui/editdataset_funcs.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/editdataset.h"

// Constructor
EditDataSetDialog::EditDataSetDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
EditDataSetDialog::~EditDataSetDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void EditDataSetDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditDataSetDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditDataSetDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditDataSetDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified reference
bool EditDataSetDialog::call(DataSet* target)
{
	// Check supplied var
	if (!target)
	{
		Messenger::print("Internal Error: NULL DataSet pointer given to EditDataSetDialog::call().\n");
		return false;
	}

	// Copy supplied data for editing
	dataSet_ = (*target);

	refreshing_ = true;
	
	// Update table
	ui.DataTable->clearContents();
	ui.DataTable->setRowCount(dataSet_.data().nValues());

	const Array<double>& x = dataSet_.x();
	const Array<double>& y = dataSet_.y();
	QTableWidgetItem* item;
	for (int n=0; n<dataSet_.data().nValues(); ++n)
	{
		item = new QTableWidgetItem(QString::number(x.constAt(n)));
		item->setFlags(Qt::ItemIsSelectable);
		ui.DataTable->setItem(n, 0, item);

		item = new QTableWidgetItem(QString::number(y.constAt(n)));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui.DataTable->setItem(n, 1, item);
	}

	refreshing_ = false;

	// Call the dialog
	if (exec()) return true;
	else return false;
}

// Get current data stored in dialog
DataSet& EditDataSetDialog::dataSet()
{
	return dataSet_;
}

/*
 * Slots
 */

void EditDataSetDialog::on_DataTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// Get item
	QTableWidgetItem* item = ui.DataTable->item(row, column);
	if (!item) return;
	
	// Set relevant entry in dataSet_
	if (column == 0) dataSet_.setX(row, item->text().toDouble());
	else if (column == 1) dataSet_.setY(row, item->text().toDouble());
}
