/*
	*** Import Dialog - Functions 
	*** src/gui/uchroma/gui/import_funcs.cpp
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

#include "gui/uchroma/gui/import.h"
#include "gui/uchroma/uchromabase.h"
#include <QFileDialog>

// Constructor
ImportDialog::ImportDialog(UChromaBase& uChromaBase, QWidget* parent) : QDialog(parent), uChromaBase_(uChromaBase)
{
	// Call the main creation function
	ui.setupUi(this);

	// Set widget values
	currentDirectory_ = QDir::currentPath();
	ui.DataFileEdit->setText(currentDirectory_.path());

	// Set variable defaults
	refreshing_ = false;
}

// Destructor
ImportDialog::~ImportDialog()
{
}

// Window close event
void ImportDialog::closeEvent(QCloseEvent *event)
{
	reject();
}

// Execute the dialog
bool ImportDialog::execute(Collection* currentCollection, ViewPane* targetViewPane)
{
	ui.ImportIntoCurrentCollectionRadio->setEnabled(currentCollection);
	if (!currentCollection) ui.ImportIntoNewCollectionRadio->setChecked(true);

	importedDataSets_.clear();
	targetViewPane_ = targetViewPane;

	int result = exec();
	return result;
}

/*
 * Private Slots
 */

void ImportDialog::on_DataFileSelectButton_clicked(bool checked)
{
	QString newFile = QFileDialog::getOpenFileName(this, "Choose import file name", currentDirectory_.absolutePath(), "All files (*.*)");
	if (!newFile.isEmpty())
	{
		ui.DataFileEdit->setText(newFile);
		currentDirectory_ = newFile;
	}
}

void ImportDialog::on_DataFileEdit_textChanged(QString text)
{
	// Make a quick check on the specified file to see if it exists and is readable, and disable/enable the Import button accordingly
	QFileInfo fileInfo(text);
	ui.ImportButton->setEnabled(fileInfo.exists() && fileInfo.isReadable() && (! fileInfo.isDir()));
}

void ImportDialog::on_ImportButton_clicked(bool checked)
{
	// Get data from the specified file
	bool result = importSequentialXY();

	// Create new Collection if requested
	Collection* collection = NULL;
	if (ui.ImportIntoNewCollectionRadio->isChecked())
	{
		// Start edit state group
		uChromaBase_.beginEditStateGroup("import data into new collection");

		collection = uChromaBase_.addCollection();

		// No need to capture additional undo information
		uChromaBase_.endEditStateGroup();

		// Set name of collection
		collection->setName(qPrintable(ui.DataFileEdit->text()));

		// Add pane to specified view pane
		if (targetViewPane_) targetViewPane_->addCollectionTarget(collection);
	}
	else collection = uChromaBase_.currentCollection();

	// Check current Collection
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionDataImport_triggered()")) return;

	// Loop over list of imported slices and copy them to our local list
	for (DataSet* dataSet = importedDataSets_.first(); dataSet != NULL; dataSet = dataSet->next) collection->addDataSet(dataSet);

	// Update everything
	uChromaBase_.updateGUI();

	if (result) accept();
	else reject();
}

void ImportDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}
