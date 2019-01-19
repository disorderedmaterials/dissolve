/*
	*** Edit Fit Results Dialog
	*** src/gui/uchroma/gui/editfitresults_funcs.cpp
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

#include "gui/uchroma/gui/editfitresults.h"
#include "base/messenger.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/kernels/fit.h"

// Constructor
EditFitResultsDialog::EditFitResultsDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
EditFitResultsDialog::~EditFitResultsDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void EditFitResultsDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditFitResultsDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditFitResultsDialog::on_CloseButton_clicked(bool checked)
{
	accept();
}

// Call dialog to edit specified reference
bool EditFitResultsDialog::call(Collection* collection)
{
	// Check provided collection pointer
	collection_ = collection;
	if (!Collection::objectValid(collection_, "collection in EditFitResultsDialog::call()")) return false;

	refreshing_ = true;

	// Populate table...

	// Check that we have a fitKernel...
	if (!collection_->fitKernel())
	{
		Messenger::print("EditFitResultsDialog::call() - Collection '%s' contains no FitKernel.\n", qPrintable(collection_->name()));
		return false;
	}

	// List of different value names we encounter
	List<NamedValue> valueNames;
	NamedValue* valueName;

	ui.FitResultsTable->clear();
	ui.FitResultsTable->setRowCount(collection_->fitKernel()->nDataSpaceRanges());

	// Determine nunmber of unique value names...
	for (DataSpaceRange* range = collection_->fitKernel()->dataSpaceRanges(); range != NULL; range = range->next)
	{
		// Loop over variables in range...
		for (NamedValue* value = range->fittedValues(); value != NULL; value = value->next)
		{
			// Search for NamedValue in the valueNames list...
			for (valueName = valueNames.first(); valueName != NULL; valueName = valueName->next) if (value->name() == valueName->name()) break;
			if (!valueName) valueNames.add()->setName(value->name());
		}
	}

	// Now add items to table...
	ui.FitResultsTable->setColumnCount(valueNames.nItems() + 1);
	QTableWidgetItem* item;
	int row = 0, column;
	for (DataSpaceRange* range = collection_->fitKernel()->dataSpaceRanges(); range != NULL; range = range->next)
	{
		// Create 'index' entry
		item = new QTableWidgetItem();
		if (collection_->fitKernel()->global()) item->setText("Global");
		else if (collection_->fitKernel()->orthogonal()) item->setText("X = "+QString::number(range->xStart()));
		else
		{
			DataSet* dataSet = collection_->fitKernel()->sourceCollection()->dataSet(range->displayDataSetStart());
			item->setText(dataSet ? dataSet->name() : "???");
		}
		ui.FitResultsTable->setItem(row, 0, item);
		
		for (NamedValue* value = range->fittedValues(); value != NULL; value = value->next)
		{
			// Search for NamedValue in the valueNames list...
			for (valueName = valueNames.first(); valueName != NULL; valueName = valueName->next) if (value->name() == valueName->name()) break;
			if (!valueName) continue;

			// Get column for item, and add it in
			column = valueNames.indexOf(valueName) + 1;
			item = new QTableWidgetItem();
			item->setText(QString::number(value->value(), 'e'));
			ui.FitResultsTable->setItem(row, column, item);
		}
		
		++row;
	}

	// Set horizontal headers
	QStringList headerItems;
	headerItems << "Fit Range";
	for (NamedValue* value = valueNames.first(); value != NULL; value = value->next) headerItems << value->name();
	ui.FitResultsTable->setHorizontalHeaderLabels(headerItems);

	refreshing_ = false;

	// Call the dialog
	if (exec()) return true;
	else return false;
}

/*
 * Slots
 */
