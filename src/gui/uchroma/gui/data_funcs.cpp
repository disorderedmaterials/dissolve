/*
	*** Data Window
	*** src/gui/uchroma/gui/data_funcs.cpp
	Copyright T. Youngs 2013-2017

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

#include "gui/uchroma/gui/data.h"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/gui/editdataset.h"
#include "gui/uchroma/gui/operate_setz.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>

/*
 * Window Functions
 */

// Constructor
DataWindow::DataWindow(UChromaBase& uChromaBase, QWidget* parent) : uChromaBase_(uChromaBase), QWidget(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
DataWindow::~DataWindow()
{
}

// Window close event
void DataWindow::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

// Reload datasets from their associated files
void DataWindow::reloadDataSets()
{
	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::reloadDataSets()")) return;

	QString dir = QFileDialog::getExistingDirectory(this, "Select Data Directory", currentCollection->dataFileDirectory().path());
	if (dir.isEmpty()) return;
	ui.SourceDirEdit->setText(dir);
	currentCollection->setDataFileDirectory(dir);
	
	// Reload all data and update surface
	QProgressDialog progress("Loading data...", "Abort", 0, currentCollection->nDataSets(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		currentCollection->loadDataSet(dataSet);
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nDataSets());

	// Need to update GUI
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();
}

/*
 * Slots
 */

// Select source directory
void DataWindow::on_SourceDirSelectButton_clicked(bool checked)
{
	reloadDataSets();
}

// Add files button clicked
void DataWindow::on_AddFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::on_AddFilesButton_clicked()")) return;

	QStringList files = QFileDialog::getOpenFileNames(this, QString("Select XY datafiles (importing into '")+currentCollection->name()+"')", currentCollection->dataFileDirectory().path(), "Text files (*.txt);;MINT files (*.mint01);;MDCS files (*.mdcs01);;All files (*)");

	QProgressDialog progress("Loading data...", "Abort Loading", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);
	int count = 0;

	// Loop	over filenames and append datasets
	foreach (QString fileName, files)
	{
		progress.setValue(count);
		if (progress.wasCanceled()) break;

		if (currentCollection->appendDataSet(qPrintable(fileName))) ++count;
	}
	progress.setValue(files.count());

	// Was any data loaded?
	if (count == 0) return;

	// Query whether limits should be updated to encompass all data
	if (QMessageBox::question(this, "New Data Loaded", "New data has been loaded - expand limits to encompass all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
	{
		// Get list of panes that are currently displaying this collection...
		RefList<ViewPane,bool> panes = uChromaBase_.viewLayout().panes(currentCollection, ViewPane::StandardRole);
		for (RefListItem<ViewPane,bool>* ri = panes.first(); ri != NULL; ri = ri->next) ri->item->showAllData();
	}

	// Need to update GUI
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();
}

// Remove files button clicked
void DataWindow::on_RemoveFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::on_RemoveFilesButton_clicked()")) return;

	// From the selected items, construct a list of dataset to remove
	RefList<DataSet,int> dataSetsToRemove;
	foreach(QTableWidgetItem* item, ui.DataSetsTable->selectedItems()) dataSetsToRemove.addUnique(currentCollection->dataSet(item->row()));

	// Delete datasets....
	for (RefListItem<DataSet,int>* ri = dataSetsToRemove.first(); ri != NULL; ri = ri->next) currentCollection->removeDataSet(ri->item);

	// Need to update GUI
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();
}

// Source data item selection changed
void DataWindow::on_DataSetsTable_itemSelectionChanged()
{
	bool nSelected = ui.DataSetsTable->selectedItems().count();
	ui.RemoveFilesButton->setEnabled(nSelected > 0);
	ui.EditDataSetButton->setEnabled(nSelected > 0);
}

void DataWindow::on_DataSetsTable_itemDoubleClicked(QTableWidgetItem* item)
{
	if (item->column() == 0)
	{
		DataSet* dataSet = VariantPointer<DataSet>(item->data(Qt::UserRole));
		if (!dataSet) return;

		EditDataSetDialog dataSetDialog(this);
		if (dataSetDialog.call(dataSet)) uChromaBase_.currentCollection()->setDataSetData(dataSet, dataSetDialog.dataSet());

		updateControls();
	}
}

void DataWindow::on_DataSetsTable_cellChanged(int row, int column)
{
	if (refreshing_) return;

	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::on_DataSetsTable_cellChanged()")) return;

	// Z changed
	if (column == 2)
	{
		// Get slice and widget item
		DataSet* dataSet = currentCollection->dataSet(row);
		if (dataSet == NULL) return;

		QTableWidgetItem* item = ui.DataSetsTable->item(row, column);
		if (item == NULL) return;

		// Set new value of z (its position in the list will be adjusted if necessary)
		currentCollection->setDataSetZ(dataSet, item->text().toDouble());

		// Need to update now
		uChromaBase_.setAsModified();
		uChromaBase_.updateGUI();
	}
}

void DataWindow::on_SetZButton_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::on_SetZButton_clicked()")) return;

	// Check for no datasets
	if (currentCollection->nDataSets() == 0) return;

	// Create a DataSetZDialog and show it...
	OperateSetZDialog setZDialog(this, currentCollection);
	if (setZDialog.exec())

	// Need to update now
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();
}

void DataWindow::on_ReloadFilesButton_clicked(bool checked)
{
	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::on_ReloadFilesButton_clicked()")) return;
	
	// Reload all data
	QProgressDialog progress("Reloading data...", "Abort", 0, currentCollection->nDataSets(), this);
	progress.setWindowModality(Qt::WindowModal);
	int n=0;
	int nFailed = 0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		if (!currentCollection->loadDataSet(dataSet)) ++nFailed;
		progress.setValue(n++);
		if (progress.wasCanceled()) break;
	}
	progress.setValue(currentCollection->nDataSets());

	// Any failed to load?
	if (nFailed > 0)
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Failed to Load Data", QString("Failed to reload data for ") + QString::number(nFailed) + " defined datasets.\nWould you like to remove empty datasets from the list?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		if (button == QMessageBox::Yes)
		{
			DataSet* dataSet = currentCollection->dataSets(), *nextDataSet;
			while (dataSet != NULL)
			{
				nextDataSet = dataSet->next;
				if (dataSet->data().nPoints() == 0) currentCollection->removeDataSet(dataSet);
				dataSet = nextDataSet;
			}
		}
	}

	// Need to update GUI
	uChromaBase_.setAsModified();
	uChromaBase_.updateGUI();
}

void DataWindow::on_EditDataSetButton_clicked(bool checked)
{
	// From the selected items, construct a list of dataset to remove
	RefList<DataSet,int> dataSetsToEdit;
	foreach(QTableWidgetItem* item, ui.DataSetsTable->selectedItems()) dataSetsToEdit.addUnique(VariantPointer<DataSet>(item->data(Qt::UserRole)));

	// Raise a dialog for each selected dataset in turn
	for (RefListItem<DataSet,int>* ri = dataSetsToEdit.first(); ri != NULL; ri = ri->next)
	{
		DataSet* dataSet = ri->item;
		if (!dataSet) continue;

		EditDataSetDialog dataSetDialog(this);
		if (dataSetDialog.call(dataSet)) uChromaBase_.currentCollection()->setDataSetData(dataSet, dataSetDialog.dataSet());
	}

	updateControls();
}

/*
 * Update
 */

// Update controls and show window
void DataWindow::updateAndShow()
{
	updateControls(true);
	show();
	move(uChromaBase_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void DataWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Check for valid collection
	Collection* currentCollection = uChromaBase_.currentCollection();
	if (!Collection::objectValid(currentCollection, "collection in DataWindow::updateControls()")) return;

	refreshing_ = true;

	ui.SourceDirEdit->setText(currentCollection->dataFileDirectory().absolutePath());

	ui.DataSetsTable->clearContents();
	ui.DataSetsTable->setRowCount(currentCollection->nDataSets());
	int count = 0;
	for (DataSet* dataSet = currentCollection->dataSets(); dataSet != NULL; dataSet = dataSet->next)
	{
		// Create filename item
		QTableWidgetItem* item = new QTableWidgetItem(dataSet->sourceFileName());
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 0, item);

		// Create nPoints item
		item = new QTableWidgetItem(QString::number(dataSet->data().nPoints()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 1, item);

		// Create Z value item
		item = new QTableWidgetItem(QString::number(dataSet->data().constZ()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		ui.DataSetsTable->setItem(count, 2, item);

		// Create Source item
		item = new QTableWidgetItem(DataSet::dataSource(dataSet->dataSource()));
		item->setData(Qt::UserRole, VariantPointer<DataSet>(dataSet));
		item->setFlags(Qt::ItemIsSelectable);
		ui.DataSetsTable->setItem(count, 3, item);

		++count;
	}
	ui.DataSetsTable->resizeColumnsToContents();

	refreshing_ = false;
}
