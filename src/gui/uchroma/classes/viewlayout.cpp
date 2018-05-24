/*
	*** View Layout
	*** src/gui/uchroma/classes/viewlayout.cpp
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

#include "gui/uchroma/classes/viewlayout.h"

// Constructor
ViewLayout::ViewLayout(UChromaBase& uChromaBase) : ListItem<ViewLayout>(), uChromaBase_(uChromaBase)
{
	nColumns_ = 1;
	nRows_ = 1;
	layoutXOffset_ = 0;
	layoutYOffset_ = 0;
	layoutXScale_ = 1.0;
	layoutYScale_ = 1.0;
	pixelWidth_ = 0;
	pixelHeight_ = 0;
	remainingWidth_ = 0;
	remainingHeight_ = 0;
}

// Destructor
ViewLayout::~ViewLayout()
{
}

// Copy constructor
ViewLayout::ViewLayout(const ViewLayout& source) : uChromaBase_(source.uChromaBase_)
{
	(*this) = source;
}

// Assignment operator
void ViewLayout::operator=(const ViewLayout& source)
{
	// Clear current data
	panes_.clear();

	// Copy source data
	name_ = source.name_;
	nColumns_ = source.nColumns_;
	nRows_ = source.nRows_;
	layoutXOffset_ = source.layoutXOffset_;
	layoutYOffset_ = source.layoutYOffset_;
	layoutXScale_ = source.layoutXScale_;
	layoutYScale_ = source.layoutYScale_;
	pixelWidth_ = source.pixelWidth_;
	pixelHeight_ = source.pixelHeight_;
	remainingWidth_ = source.remainingWidth_;
	remainingHeight_ = source.remainingHeight_;

	// Copy panes...
	for (ViewPane* pane = source.panes_.first(); pane != NULL; pane = pane->next)
	{
		ViewPane* newPane = panes_.add(*this);
		(*newPane) = (*pane);
	}
}

/*
 * Link to UChromaBase
 */

// Return reference to UChromaBase
UChromaBase& ViewLayout::uChromaBase()
{
	return uChromaBase_;
}

/*
 * Layout
 */

// Recalculate pixel dimensions and remainder
void ViewLayout::recalculatePixels()
{
	// Set new pixel widths
	pixelWidth_ = layoutWidth_ / nColumns_;
	pixelHeight_ = layoutHeight_ / nRows_;

	// Recalculate pane sizes
	remainingWidth_ = layoutWidth_ - nColumns_*pixelWidth_;
	remainingHeight_ = layoutHeight_ - nRows_*pixelHeight_;
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next)
	{
		pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
		pane->recalculateView();

		// Apply pixel offsets to each pane
		pane->translateViewport(layoutXOffset_, layoutYOffset_);
	}
}

// Clear layout data
void ViewLayout::clear()
{
	name_ = "<New Layout>";
	nColumns_ = 4;
	nRows_ = 4;
	panes_.clear();
}

// Set default layout
ViewPane* ViewLayout::setDefault()
{
	clear();

	setName("Default");
	setGrid(4, 4);
	ViewPane* pane = addPane("Main view", 0, 0, 4, 4);
	return pane;
}

// Set name
void ViewLayout::setName(const char* name)
{
	name_ = name;
}

// Set number of rows and columns
void ViewLayout::setGrid(int nColumns, int nRows)
{
	nColumns_ = nColumns;
	nRows_ = nRows;

	recalculatePixels();
}

// Return number of columns in layout
int ViewLayout::nColumns() const
{
	return nColumns_;
}

// Return number of rows in layout
int ViewLayout::nRows() const
{
	return nRows_;
}

// Set pixel offsets and scales to use in layout
void ViewLayout::setOffsetAndScale(int xOffset, int yOffset, double xScale, double yScale)
{
	layoutXOffset_ = xOffset;
	layoutYOffset_ = yOffset;
	layoutXScale_ = xScale;
	layoutYScale_ = yScale;
}

// Recalculate layout
void ViewLayout::recalculate(int contextWidth, int contextHeight)
{
	layoutWidth_ = contextWidth * layoutXScale_;
	layoutHeight_ = contextHeight * layoutYScale_;

	recalculatePixels();
}

/*
 * Pane Functions
 */

// Return unique name based on supplied basename
CharString ViewLayout::uniqueViewPaneName(const char* baseName)
{
	CharString testName = baseName, suffix;
	int index = 0;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName.sprintf("%s (%i)", baseName, index);
		++index;
	} while (pane(testName));

	return testName;
}

// Add pane to layout
ViewPane* ViewLayout::addPane(const char* name, int left, int top, int width, int height)
{
	ViewPane* pane = panes_.add(*this);
	pane->setName(uniqueViewPaneName(name));
	pane->setSize(width, height);

	pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);

	return pane;
}

// Remove pane from layout
void ViewLayout::removePane(ViewPane* pane)
{
	if (!panes_.contains(pane))
	{
		Messenger::print("Internal Error: Tried to remove a pane from a ViewLayout that doesn't own it.\n");
		return;
	}

	panes_.remove(pane);
}

// Return list of panes
ViewPane* ViewLayout::panes()
{
	return panes_.first();
}

// Return last pane in list
ViewPane* ViewLayout::lastPane()
{
	return panes_.last();
}

// Return named pane (if it exists)
ViewPane* ViewLayout::pane(const char* name)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (DissolveSys::sameString(pane->name(), name)) return pane;
	return NULL;
}

// Return index of specified pane in list
int ViewLayout::paneIndex(ViewPane* pane)
{
	return panes_.indexOf(pane);
}

// Return reflist of all panes of specified type
RefList<ViewPane,bool> ViewLayout::panes(ViewPane::PaneRole role)
{
	RefList<ViewPane,bool> result;
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) if (pane->role() == role) result.add(pane);
	return result;
}

// Return reflist of panes (optionally of specified type) that target specified collection
RefList<ViewPane,bool> ViewLayout::panes(Collection* collection, ViewPane::PaneRole role)
{
	RefList<ViewPane,bool> result;
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next)
	{
		if ((role != ViewPane::nPaneRoles) && (pane->role() != role)) continue;
		if (pane->collectionIsTarget(collection)) result.add(pane);
	}
	return result;
}

// Return whether pane is used anywhere on a pane (optionally only of specified type)
ViewPane* ViewLayout::collectionUsed(Collection* collection, ViewPane::PaneRole role)
{
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next)
	{
		if ((role != ViewPane::nPaneRoles) && (pane->role() != role)) continue;
		if (pane->collectionIsTarget(collection)) return pane;
	}
	return NULL;
}

// Return if pane is in the current list
bool ViewLayout::containsPane(ViewPane* pane)
{
	return panes_.contains(pane);
}

// Return pane under specified coordinate
ViewPane* ViewLayout::paneAt(int layoutX, int layoutY)
{
	// Search in reverse order, since this reflects the 'stack' when drawn
	for (ViewPane* pane = panes_.last(); pane != NULL; pane = pane->prev) if (pane->containsCoordinate(layoutX, layoutY)) return pane;
	return NULL;
}

// Return pane containing specified grid reference
ViewPane* ViewLayout::paneAtGrid(int gridX, int gridY)
{
	// Search in reverse order, since this reflects the 'stack' when drawn
	for (ViewPane* pane = panes_.last(); pane != NULL; pane = pane->prev) if (pane->containsGridReference(gridX, gridY)) return pane;
	return NULL;
}

// Translate pane by the amount specified
void ViewLayout::translatePane(ViewPane* pane, int deltaX, int deltaY)
{
	// Check that a meaningful delta was supplied
	if ((deltaX == 0) && (deltaY == 0)) return;

	pane->setBottomLeft(pane->bottomEdge()+deltaY, pane->leftEdge()+deltaX);
	pane->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
}

// Bring pane to front
void ViewLayout::bringPaneToFront(ViewPane* pane, bool onTop)
{
	// Bringing to front actually means send to tail of list, since this is pane drawing order
	if (onTop) panes_.moveToEnd(pane);
	else panes_.shiftDown(pane);
}

// Send pane to back
void ViewLayout::sendPaneToBack(ViewPane* pane, bool onBottom)
{
	// Sending to back actually means bring to head of list, since this is pane drawing order
	if (onBottom) panes_.moveToStart(pane);
	else panes_.shiftUp(pane);
}

// Pane has changed
void ViewLayout::paneChanged(ViewPane* caller)
{
// 	printf("Caller = %p, %i %i %i %i %i %i\n", caller, pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
	if (caller) caller->recalculateViewport(pixelWidth_, pixelHeight_, nColumns_, nRows_, remainingWidth_, remainingHeight_);
}

// Reset view of all panes
void ViewLayout::resetViewMatrix()
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->resetViewMatrix();
}

// Update interaction primitives for all panes
void ViewLayout::updateInteractionPrimitives(int axis)
{
	// Loop over view panes
	for (ViewPane* pane = panes_.first(); pane != NULL; pane = pane->next) pane->updateInteractionPrimitive(axis);
}
