/*
	*** uChroma Full Widget - Slots
	*** src/gui/uchroma/gui/uchromafull_slots.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/gui/uchromafull.h"
#include "gui/uchroma/gui/editviewlayout.h"
#include "gui/uchroma/gui/editfitresults.h"
#include "gui/uchroma/gui/operate_bgsub.h"
#include "gui/uchroma/gui/operate_setz.h"
#include "gui/uchroma/render/fontinstance.h"
#include "templates/reflist.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QProgressDialog>
#include <QPainter>

// Window close event
void UChromaFullWindow::closeEvent(QCloseEvent *event)
{
	if (checkBeforeClose()) event->accept();
	else event->setAccepted(false);
}

/*
 * File Actions
 */

void UChromaFullWindow::on_actionFileNewSession_triggered(bool checked)
{
	if (isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (isModified()) return;
		}
	}

	startNewSession(true);

	// Update the GUI
	updateGUI();
}

void UChromaFullWindow::on_actionFileLoadSession_triggered(bool checked)
{
	if (isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (isModified()) return;
		}
	}

	QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Load input file
	if (!loadSession(qPrintable(fileName)))
	{
		QMessageBox::information(this, "Failed to load session", "Failed to load the session, so reverting to the default.\n");
		startNewSession(true);
	}

	// Update the GUI
	updateGUI();
}

void UChromaFullWindow::on_actionFileSaveSession_triggered(bool checked)
{
	// Has an input filename already been chosen?
	if (inputFile_.isEmpty())
	{
		QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
		if (fileName.isEmpty()) return;

		// Make sure the file has the right extension
		QFileInfo fileInfo(fileName);
		if (fileInfo.suffix() != "ucr") fileName += ".ucr";
		setInputFile(qPrintable(fileName));
	}

	if (saveSession(inputFile())) setAsNotModified();
}

void UChromaFullWindow::on_actionFileSaveSessionAs_triggered(bool checked)
{
	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose save file name", sessionFileDirectory().absolutePath(), "uChroma files (*.ucr);;All files (*.*)");
	if (fileName.isEmpty()) return;

	// Make sure the file has the right extension
	QFileInfo fileInfo(fileName);
	if (fileInfo.suffix() != "ucr") fileName += ".ucr";
	setInputFile(qPrintable(fileName));

	if (saveSession(inputFile())) setAsNotModified();
}

void UChromaFullWindow::on_actionFilePrint_triggered(bool checked)
{
	// TODO
}

void UChromaFullWindow::on_actionFileExportImage_triggered(bool checked)
{
	static bool firstRun = true;
	if (saveImageDialog_.getImageDetails(double(ui.MainView->width()) / double(ui.MainView->height())))
	{
		// Check to see if existing image file already exists
		if (QFile::exists(imageExportFileName()))
		{
			if (QMessageBox::question(this, "File Exists", QString("The file '") + imageExportFileName() + "' already exists.\nOverwrite it?", QMessageBox::No | QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) return;
		}

		int imageWidth = imageExportWidth();
		int imageHeight = imageExportHeight();

		// Generate and save pixmap
		QPixmap pixmap = ui.MainView->generateImage(imageWidth, imageHeight);
		pixmap.save(imageExportFileName(), imageFormatExtension(imageExportFormat()), -1);
	}

	firstRun = false;
}

void UChromaFullWindow::on_actionFileQuit_triggered(bool checked)
{
	if (checkBeforeClose()) QApplication::exit(0);
}

// Check for modified data before closing
bool UChromaFullWindow::checkBeforeClose()
{
	if (isModified())
	{
		QMessageBox::StandardButton button = QMessageBox::warning(this, "Warning", "The current file has been modified.\nDo you want to save this data first?", QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No, QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) return false;
		else if (button == QMessageBox::Yes)
		{
			// Save file, and check isModified() status to make sure it wasn't cancelled.
			on_actionFileSaveSession_triggered(false);
			if (isModified()) return false;
		}
	}

	return true;
}

/*
 * Edit Actions
 */
void UChromaFullWindow::on_actionEditUndo_triggered(bool checked)
{
	undo();

	updateGUI();
}

void UChromaFullWindow::on_actionEditRedo_triggered(bool checked)
{
	redo();

	updateGUI();
}

/*
 * View Actions
 */

bool UChromaFullWindow::viewTypeChanged(ViewPane::ViewType vt)
{
	currentViewPane()->setViewType(vt);
	currentViewPane()->resetViewMatrix();
	currentViewPane()->recalculateView(true);

	setAsModified();

	axesWindow_.updateControls();
	updateToolBars();
	ui.MainView->update();

	return true;
}

void UChromaFullWindow::on_actionViewPerspective_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewPerspective_triggered()")) return;
	
	currentViewPane()->setHasPerspective(checked);

	setAsModified();

	ui.MainView->update();
}

void UChromaFullWindow::on_actionViewReset_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewReset_triggered()")) return;

	currentViewPane()->resetViewMatrix();
	currentViewPane()->recalculateView();

	setAsModified();

	ui.MainView->update();
}

void UChromaFullWindow::on_actionViewShowAll_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewShowAll_triggered()")) return;

	currentViewPane()->showAllData();

	setAsModified();

	updateGUI();
}

void UChromaFullWindow::on_actionViewNormal_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewNormal_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::NormalView);
}

void UChromaFullWindow::on_actionViewAutoStretched3D_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewAutoStretched3D_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::AutoStretchedView);
}

void UChromaFullWindow::on_actionViewFlatXY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewFlatXY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXYView);
}

void UChromaFullWindow::on_actionViewFlatXZ_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewFlatXZ_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXZView);
}

void UChromaFullWindow::on_actionViewFlatZY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewFlatZY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatZYView);
}

void UChromaFullWindow::on_actionViewLinked_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaFullWindow::on_actionViewLinked_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::LinkedView);
}

void UChromaFullWindow::on_actionViewAxes_triggered(bool checked)
{
	ui.actionWindowAxes->trigger();
}

void UChromaFullWindow::on_actionViewChangeLayout_triggered(bool checked)
{
	EditViewLayoutDialog layoutDialog(*this, this);

	// Call the dialog
	if (layoutDialog.call(viewLayout()))
	{
		setViewLayout(layoutDialog.viewLayout());
		viewLayout().recalculate(ui.MainView->contextWidth(), ui.MainView->contextHeight());

		setAsModified();

		updateGUI();
	}
}

/*
 * Collection Actions
 */

void UChromaFullWindow::on_actionCollectionFocusNext_triggered(bool checked)
{
	focusNextCollection();
}

void UChromaFullWindow::on_actionCollectionFocusPrevious_triggered(bool checked)
{
	focusPreviousCollection();
}

void UChromaFullWindow::on_actionCollectionNew_triggered(bool checked)
{
	// Start edit state group
	beginEditStateGroup("create new collection");

	addCollection();

	// End edit state group
	endEditStateGroup();

	updateGUI();
}

void UChromaFullWindow::on_actionCollectionCreate_triggered(bool checked)
{
	createCollectionDialog_.updateAndShow();
}

void UChromaFullWindow::on_actionCollectionDuplicate_triggered(bool checked)
{
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "current collection in UChromaFullWindow::on_actionCollectionDuplicate_triggered")) return;

	Collection* newCollection;
	if (collection->parent())
	{
		if (collection->type() == Collection::FitCollection) newCollection = collection->parent()->addFit(collection->name());
		else if (collection->type() == Collection::ExtractedCollection) newCollection = collection->parent()->addSlice(collection->name());
		else return;
	}
	else newCollection = addCollection(collection->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*collection);
	newCollection->setName(qPrintable(name));

	// Create editstate
	beginEditStateGroup("duplicate collection '%s'", qPrintable(collection->name()));
	if (addEditState(newCollection->objectInfo(), EditState::CollectionAddQuantity))
	{
		addEditStateData(true, "collection", newCollection);
		addEditStateData(true, "locator", newCollection->locator());
		addEditStateData(true, "position", collectionIndex(newCollection));
	}
	endEditStateGroup();

	updateGUI();
}

void UChromaFullWindow::on_actionCollectionPromoteToMaster_triggered(bool checked)
{
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "current collection in UChromaFullWindow::on_actionCollectionPromoteToMaster_triggered")) return;

	if (collection->type() == Collection::MasterCollection) return;

	Collection* newCollection = addCollection(collection->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*collection);
	newCollection->setName(qPrintable(name));

	updateGUI();
}

void UChromaFullWindow::on_actionCollectionStyle_triggered(bool checked)
{
	ui.actionWindowStyle->trigger();
}

void UChromaFullWindow::on_actionCollectionTransform_triggered(bool checked)
{
	ui.actionWindowTransform->trigger();
}

void UChromaFullWindow::on_actionCollectionDelete_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionCollectionDelete_triggered()")) return;

	// Start edit state group
	beginEditStateGroup("delete collection '%s'", qPrintable(collection->name()));

	// Remove specified collection
	removeCollection(collection);

	// Add new empty collection if there is now no current collection
	if (currentCollection() == NULL) addCollection();

	// End edit state group
	endEditStateGroup();

	// Update GUI
	updateGUI();
}

void UChromaFullWindow::on_actionCollectionExport_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionCollectionDelete_triggered()")) return;

	static QDir currentDirectory = sessionFileDirectory();

	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose export file name", currentDirectory.absolutePath(), "All files (*.*)");
	if (fileName.isEmpty()) return;

	collection->exportData(qPrintable(fileName));

	currentDirectory = fileName;
}

/*
 * Data Actions
 */

void UChromaFullWindow::on_actionDataLoadXY_triggered(bool checked)
{
	// Check current Collection
	if (!Collection::objectValid(currentCollection(), "collection in UChromaFullWindow::on_actionDataLoadXY_triggered()")) return;

	dataWindow_.ui.AddFilesButton->click();
}

void UChromaFullWindow::on_actionDataImport_triggered(bool checked)
{
	importDialog_.execute(currentCollection_, currentViewPane());
}

void UChromaFullWindow::on_actionDataReload_triggered(bool checked)
{
	dataWindow_.reloadDataSets();
}

void UChromaFullWindow::on_actionDataView_triggered(bool checked)
{
	ui.actionWindowData->trigger();
}

/*
 * Operate Actions
 */

void UChromaFullWindow::on_actionOperateConstantBackgroundSubtraction_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionOperateConstantBackgroundSubtraction_triggered()")) return;

	OperateBGSubDialog bgSubDialog(*this, this);
	if (bgSubDialog.updateAndExec(collection)) updateGUI();
}

void UChromaFullWindow::on_actionOperateSetZValues_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionOperateSetZValues_triggered()")) return;

	OperateSetZDialog setZDialog(this, collection);
	if (setZDialog.exec()) updateGUI();
}

/*
 * Analyse Actions
 */

void UChromaFullWindow::on_actionAnalyseNewFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionAnalyseNewFit_triggered()")) return;

	// Add a new fit to the current collection
	Collection* newFit = collection->addFit("New Fit");

	// Raise the EditFitSetup dialog
	editFitKernelDialog_.setFitKernel(newFit->fitKernel());
	if (editFitKernelDialog_.updateAndExec())
	{
		// Add new fit data to same pane as parent (if possible)
		RefList<ViewPane,bool> parentPanes = viewLayout().panes(collection, ViewPane::StandardRole);
		if (parentPanes.contains(currentViewPane())) currentViewPane()->addCollectionTarget(newFit);
		else if (parentPanes.nItems() != 0) parentPanes.first()->item->addCollectionTarget(newFit);

		// Set current collection to be the new fit
		setCurrentCollection(newFit);
	}
	else collection->removeFit(newFit);

	updateGUI();
}

void UChromaFullWindow::on_actionAnalyseEditFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionAnalyseEditFit_triggered()")) return;

	if (collection->fitKernel())
	{
		editFitKernelDialog_.setFitKernel(collection->fitKernel());
		if (editFitKernelDialog_.updateAndExec()) updateGUI();
	}
	else
	{
		if (QMessageBox::question(this, "No Fit to Edit", "The current collection does not contain any fit parameters.\nWould you like to create a new fit?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) on_actionAnalyseNewFit_triggered(false);
	}
}

void UChromaFullWindow::on_actionAnalyseUpdateFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionAnalyseUpdateFit_triggered()")) return;

	if (collection->fitKernel())
	{
		if (collection->fitKernel()->fit()) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(collection->name()));
		Messenger::print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
	}
}

void UChromaFullWindow::on_actionAnalyseResetAndRestartFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionAnalyseResetAndRestartFit_triggered()")) return;

	if (collection->fitKernel())
	{
		if (collection->fitKernel()->fit(true)) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(collection->name()));
		Messenger::print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
	}
}

void UChromaFullWindow::on_actionAnalyseViewFitResults_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaFullWindow::on_actionAnalyseResetAndRestartFit_triggered()")) return;

	if (collection->fitKernel())
	{
		EditFitResultsDialog fitValuesDialog(this);
		fitValuesDialog.call(collection);
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(collection->name()));
		Messenger::print(qPrintable(message));
		ui.StatusBar->showMessage(message, 3000);
	}
}

/*
 * Interact Actions
 */

// Change interaction axis target
void UChromaFullWindow::interactionActionTriggered(int axis)
{
	// Toggle interaction axis
	if ((interactionAxis_ == axis) || (axis == -1))
	{
		ui.actionInteractNone->setChecked(true);
		setInteractionMode(UChromaBase::ViewInteraction, -1);
	}
	else
	{
		setInteractionMode(UChromaBase::ZoomInteraction, axis);
		updateInteractionPosition(ui.MainView->rMouseLast().x, ui.MainView->contextHeight() - ui.MainView->rMouseLast().y);
	}

	// Update GUI
	updateDisplay();
	updateCoordinateInfo();
}

void UChromaFullWindow::on_actionInteractX_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(0);
}

void UChromaFullWindow::on_actionInteractY_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(1);
}

void UChromaFullWindow::on_actionInteractZ_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(2);
}

void UChromaFullWindow::on_actionInteractNone_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(-1);
}

/*
 * Settings Actions
 */

void UChromaFullWindow::on_actionSettingsChooseFont_triggered(bool checked)
{
	static QDir currentFontDirectory = QString(viewerFontFileName());
	QString newFont = QFileDialog::getOpenFileName(this, "Choose truetype font", currentFontDirectory.path(), "TrueType font files (*.ttf);;All files (*.*)");
	if (!newFont.isEmpty())
	{
		if (!QFile::exists(newFont)) QMessageBox::warning(this, "Font Error", "The specified font file '" + newFont + "' does not exist.");
		else if (!ui.MainView->setupFont(qPrintable(newFont))) QMessageBox::warning(this, "Font Error", "Failed to create a font from the specified font file '" + newFont +"'.");
		setViewerFontFileName(qPrintable(newFont));
		saveSettings();
	}
}

void UChromaFullWindow::on_actionSettingsResetFont_triggered(bool checked)
{
	setViewerFontFileName("");
	ui.MainView->setupFont(viewerFontFileName());
	saveSettings();
}

/*
 * Window Actions
 */

void UChromaFullWindow::on_actionWindowAxes_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
}

void UChromaFullWindow::on_actionWindowData_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) dataWindow_.updateAndShow();
	else dataWindow_.hide();
}

void UChromaFullWindow::on_actionWindowStyle_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) styleWindow_.updateAndShow();
	else styleWindow_.hide();
}

void UChromaFullWindow::on_actionWindowTransform_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) transformWindow_.updateAndShow();
	else transformWindow_.hide();
}

void UChromaFullWindow::on_actionWindowView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) viewWindow_.updateAndShow();
	else viewWindow_.hide();
}
