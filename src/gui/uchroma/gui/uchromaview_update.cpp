/*
	*** View Widget - Update Functions
	*** src/gui/uchroma/gui/uchromaview_udpate.cpp
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

#include "gui/viewer/dataviewer.hui"
#include "templates/reflist.h"
#include "templates/variantpointer.h"

// Update all aspects of GUI
void UChromaViewWidget::updateGUI()
{
	refreshing_ = true;

	updateToolBars();
	updateSubWindows();
	updateDisplay();
	updateUndoRedo();

	refreshing_ = false;
}

// Update all subwindows
void UChromaViewWidget::updateSubWindows()
{
	// Make sure limits are up to date
	ViewPane* viewPane = currentViewPane();
	if (ViewPane::objectValid(viewPane, "view pane in UChromaViewWidget::updateSubWindows()")) viewPane->updateAxisLimits();

	axesWindow_.updateControls();
	dataWindow_.updateControls();
	styleWindow_.updateControls();
	transformWindow_.updateControls();
	viewWindow_.updateControls();

	// Set titles of each subwindow to reflect current collection and viewpane
	Collection* collection = currentCollection();
	if (Collection::objectValid(collection, "collection in UChromaViewWidget::updateSubWindows"))
	{
		dataWindow_.setWindowTitle(QString("Data (") + collection->name() + ")");
		styleWindow_.setWindowTitle(QString("Style (") + collection->name() + ")");
		transformWindow_.setWindowTitle(QString("Transform (") + collection->name() + ")");
		viewWindow_.setWindowTitle(QString("View (") + collection->name() + ")");
	}
	if (ViewPane::objectValid(viewPane, "view pane in UChromaViewWidget::updateSubWindows()")) axesWindow_.setWindowTitle(QString("Axes (") + viewPane->name() + ")");
}

// Update tool bars
void UChromaViewWidget::updateToolBars()
{
	// Update controls related to current ViewPane
	ViewPane* viewPane = currentViewPane();
	if (ViewPane::objectValid(viewPane, "view pane in UChromaViewWidget::updateToolBars()"))
	{
		ui.actionViewPerspective->setChecked(viewPane->hasPerspective());
		ui.actionViewPerspective->setEnabled((viewPane->viewType() < ViewPane::FlatXYView) || (viewPane->viewType() > ViewPane::FlatZYView));
		QAction* action = viewTypeActionGroup_.actions().at(viewPane->viewType());
		if (action) action->setChecked(true);
	}
}

// Update display
void UChromaViewWidget::updateDisplay()
{
	ui.MainView->postRedisplay();
}

// Update undo/redo menu
void UChromaViewWidget::updateUndoRedo()
{
	EditStateGroup* currentUndoState = undoEditStateGroup();
	EditStateGroup* currentRedoState = (currentUndoState ? currentUndoState->next : editStateGroups());

	ui.actionEditUndo->setText(currentUndoState ? "Undo " + currentUndoState->title() : "Undo");
	ui.actionEditUndo->setEnabled(currentUndoState);

	ui.actionEditRedo->setText(currentRedoState ? "Redo " + currentRedoState->title() : "Redo");
	ui.actionEditRedo->setEnabled(currentRedoState);
}

// Update coordinate info
void UChromaViewWidget::updateCoordinateInfo()
{
	// Update controls related to current ViewPane
	ViewPane* viewPane = currentViewPane();
	if (ViewPane::objectValid(viewPane))
	{
		CharString text;
		switch (viewPane->viewType())
		{
			case (ViewPane::FlatXYView):
				text.sprintf("x = %e, y = %e", rCurrentLocal_.x, rCurrentLocal_.y);
				break;
			case (ViewPane::FlatXZView):
				text.sprintf("x = %e, z = %e", rCurrentLocal_.x, rCurrentLocal_.z);
				break;
			case (ViewPane::FlatZYView):
				text.sprintf("z = %e, y = %e", rCurrentLocal_.z, rCurrentLocal_.y);
				break;
			default:
				text.sprintf("x = %e, y = %e, z = %e", rCurrentLocal_.x, rCurrentLocal_.y, rCurrentLocal_.z);
				break;
		}

		ui.CoordinateLabel->setText(text.get());
	}
	else ui.CoordinateLabel->setText("(x,y) = (0,0)");
}

// Update text of renderTimeLabel_ in statusbar
void UChromaViewWidget::updateRenderTimeLabel(QString text)
{
}

// Update title bar of main window
void UChromaViewWidget::updateTitleBar()
{
	if (isModified()) setWindowTitle(QString("uChroma - ") + inputFile() + " (modified) ");
	else setWindowTitle(QString("uChroma - ") + inputFile());
}
