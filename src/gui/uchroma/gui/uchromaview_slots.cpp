/*
	*** View Widget - Slots
	*** src/gui/uchroma/gui/uchromaview_slots.cpp
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

#include "gui/uchroma/gui/uchromaview.h"
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

/*
 * File Actions
 */

void UChromaViewWidget::on_actionFileExportImage_triggered(bool checked)
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

/*
 * Edit Actions
 */

void UChromaViewWidget::on_actionEditUndo_triggered(bool checked)
{
	undo();

	updateGUI();
}

void UChromaViewWidget::on_actionEditRedo_triggered(bool checked)
{
	redo();

	updateGUI();
}

/*
 * View Actions
 */

bool UChromaViewWidget::viewTypeChanged(ViewPane::ViewType vt)
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

void UChromaViewWidget::on_actionViewPerspective_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewPerspective_triggered()")) return;
	
	currentViewPane()->setHasPerspective(checked);

	setAsModified();

	ui.MainView->update();
}

void UChromaViewWidget::on_actionViewReset_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewReset_triggered()")) return;

	currentViewPane()->resetViewMatrix();
	currentViewPane()->recalculateView();

	setAsModified();

	ui.MainView->update();
}

void UChromaViewWidget::on_actionViewShowAll_triggered(bool checked)
{
	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewShowAll_triggered()")) return;

	currentViewPane()->showAllData();

	setAsModified();

	updateGUI();
}

void UChromaViewWidget::on_actionViewNormal_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewNormal_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::NormalView);
}

void UChromaViewWidget::on_actionViewAutoStretched3D_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewAutoStretched3D_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::AutoStretchedView);
}

void UChromaViewWidget::on_actionViewFlatXY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewFlatXY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXYView);
}

void UChromaViewWidget::on_actionViewFlatXZ_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewFlatXZ_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatXZView);
}

void UChromaViewWidget::on_actionViewFlatZY_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewFlatZY_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::FlatZYView);
}

void UChromaViewWidget::on_actionViewLinked_triggered(bool checked)
{
	if (refreshing_) return;

	// Check current ViewPane
	if (!ViewPane::objectValid(currentViewPane(), "view pane in UChromaViewWidget::on_actionViewLinked_triggered()")) return;

	if (checked) viewTypeChanged(ViewPane::LinkedView);
}

void UChromaViewWidget::on_actionViewAxes_triggered(bool checked)
{
	ui.actionWindowAxes->trigger();
}

void UChromaViewWidget::on_actionViewChangeLayout_triggered(bool checked)
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

void UChromaViewWidget::on_actionCollectionFocusNext_triggered(bool checked)
{
	focusNextCollection();
}

void UChromaViewWidget::on_actionCollectionFocusPrevious_triggered(bool checked)
{
	focusPreviousCollection();
}

void UChromaViewWidget::on_actionCollectionNew_triggered(bool checked)
{
	// Start edit state group
	beginEditStateGroup("create new collection");

	addCollection();

	// End edit state group
	endEditStateGroup();

	updateGUI();
}

void UChromaViewWidget::on_actionCollectionCreate_triggered(bool checked)
{
	createCollectionDialog_.updateAndShow();
}

void UChromaViewWidget::on_actionCollectionDuplicate_triggered(bool checked)
{
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "current collection in UChromaViewWidget::on_actionCollectionDuplicate_triggered")) return;

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

void UChromaViewWidget::on_actionCollectionPromoteToMaster_triggered(bool checked)
{
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "current collection in UChromaViewWidget::on_actionCollectionPromoteToMaster_triggered")) return;

	if (collection->type() == Collection::MasterCollection) return;

	Collection* newCollection = addCollection(collection->name());

	// Grab name before we copy, since this will be overwritten...
	QString name = newCollection->name();
	(*newCollection) = (*collection);
	newCollection->setName(qPrintable(name));

	updateGUI();
}

void UChromaViewWidget::on_actionCollectionStyle_triggered(bool checked)
{
	ui.actionWindowStyle->trigger();
}

void UChromaViewWidget::on_actionCollectionTransform_triggered(bool checked)
{
	ui.actionWindowTransform->trigger();
}

void UChromaViewWidget::on_actionCollectionDelete_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionCollectionDelete_triggered()")) return;

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

void UChromaViewWidget::on_actionCollectionExport_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionCollectionDelete_triggered()")) return;

	static QDir currentDirectory = sessionFileDirectory();

	// Get a filename from the user
	QString fileName = QFileDialog::getSaveFileName(this, "Choose export file name", currentDirectory.absolutePath(), "All files (*.*)");
	if (fileName.isEmpty()) return;

	collection->exportData(qPrintable(fileName));

	currentDirectory = fileName;
}

/*
 * Analyse Actions
 */

void UChromaViewWidget::on_actionAnalyseNewFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionAnalyseNewFit_triggered()")) return;

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

void UChromaViewWidget::on_actionAnalyseEditFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionAnalyseEditFit_triggered()")) return;

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

void UChromaViewWidget::on_actionAnalyseUpdateFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionAnalyseUpdateFit_triggered()")) return;

	if (collection->fitKernel())
	{
		if (collection->fitKernel()->fit()) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(collection->name()));
		Messenger::print(qPrintable(message));
	}
}

void UChromaViewWidget::on_actionAnalyseResetAndRestartFit_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionAnalyseResetAndRestartFit_triggered()")) return;

	if (collection->fitKernel())
	{
		if (collection->fitKernel()->fit(true)) updateGUI();
	}
	else
	{
		QString message;
		message.sprintf("Error: Current collection '%s' has no associated fit data.\n", qPrintable(collection->name()));
		Messenger::print(qPrintable(message));
	}
}

void UChromaViewWidget::on_actionAnalyseViewFitResults_triggered(bool checked)
{
	// Check current Collection
	Collection* collection = currentCollection();
	if (!Collection::objectValid(collection, "collection in UChromaViewWidget::on_actionAnalyseResetAndRestartFit_triggered()")) return;

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
	}
}

/*
 * Interact Actions
 */

// Change interaction axis target
void UChromaViewWidget::interactionActionTriggered(int axis)
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

void UChromaViewWidget::on_actionInteractX_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(0);
}

void UChromaViewWidget::on_actionInteractY_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(1);
}

void UChromaViewWidget::on_actionInteractZ_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(2);
}

void UChromaViewWidget::on_actionInteractNone_triggered(bool checked)
{
	if (refreshing_) return;

	interactionActionTriggered(-1);
}

/*
 * Settings Actions
 */

void UChromaViewWidget::on_actionSettingsChooseFont_triggered(bool checked)
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

void UChromaViewWidget::on_actionSettingsResetFont_triggered(bool checked)
{
	setViewerFontFileName("");
	ui.MainView->setupFont(viewerFontFileName());
	saveSettings();
}

/*
 * Window Actions
 */

void UChromaViewWidget::on_actionWindowAxes_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) axesWindow_.updateAndShow();
	else axesWindow_.hide();
}

void UChromaViewWidget::on_actionWindowData_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) dataWindow_.updateAndShow();
	else dataWindow_.hide();
}

void UChromaViewWidget::on_actionWindowStyle_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) styleWindow_.updateAndShow();
	else styleWindow_.hide();
}

void UChromaViewWidget::on_actionWindowTransform_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) transformWindow_.updateAndShow();
	else transformWindow_.hide();
}

void UChromaViewWidget::on_actionWindowView_triggered(bool checked)
{
	if (refreshing_) return;
	if (checked) viewWindow_.updateAndShow();
	else viewWindow_.hide();
}
