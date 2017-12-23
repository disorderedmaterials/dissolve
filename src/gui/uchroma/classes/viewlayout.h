/*
	*** View Layout
	*** src/gui/uchroma/classes/viewlayout.h
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

#ifndef DUQ_UCHROMAVIEWLAYOUT_H
#define DUQ_UCHROMAVIEWLAYOUT_H

#include "gui/uchroma/classes/viewpane.h"
#include "gui/uchroma/classes/collection.h"
#include "templates/list.h"
#include <QString>

// Forward Declarations
/* none */

// ViewLayout
class ViewLayout : public ListItem<ViewLayout>
{
	public:
	// Constructor / Destructor
	ViewLayout(UChromaBase& uChromaBase);
	~ViewLayout();
	// Copy constructor
	ViewLayout(const ViewLayout& source);
	// Assignment operator
	void operator=(const ViewLayout& source);


	/*
	 * Link to UChromaBase
	 */
	private:
	// Link to UChromaBase
	UChromaBase& uChromaBase_;

	public:
	// Return reference to UChromaBase
	UChromaBase& uChromaBase();


	/*
	 * Layout
	 */
	private:
	// Layout name
	QString name_;
	// Number of columns in layout
	int nColumns_;
	// Number of rows in layout
	int nRows_;
	// Width of layout (in pixels)
	int layoutWidth_;
	// Height of layout (in pixels)
	int layoutHeight_;
	// Horizontal offset (in pixels)
	int layoutXOffset_;
	// Vertical offset (in pixels)
	int layoutYOffset_;
	// Horizontal scaling factor
	double layoutXScale_;
	// Vertical scaling factor
	double layoutYScale_;
	// Width of grid pixel (in pixels)
	int pixelWidth_;
	// Height of grid pixel (in pixels_
	int pixelHeight_;
	// Remaining width at right edge
	int remainingWidth_;
	// Remaining height at top edge
	int remainingHeight_;

	private:
	// Recalculate pixel dimensions and remainder
	void recalculatePixels();

	public:
	// Clear layout data
	void clear();
	// Set default layout data
	ViewPane* setDefault();
	// Set name
	void setName(QString name);
	// Set number of rows and columns
	void setGrid(int nColumns, int nRows);
	// Return number of columns in layout
	int nColumns() const;
	// Return number of rows in layout
	int nRows() const;
	// Set pixel offsets and scales to use in layout
	void setOffsetAndScale(int xOffset, int yOffset, double xScale, double yScale);
	// Set new layout size
	void recalculate(int contextWidth, int contextHeight);


	/*
	 * Pane Functions
	 */
	private:
	// List of panes in this layout
	ParentList<ViewPane,ViewLayout> panes_;

	public:
	// Return unique pane name based on name supplied
	QString uniqueViewPaneName(QString basename);
	// Add pane to layout
	ViewPane* addPane(QString name = QString(), int left = 0, int top = 0, int width = 1, int height = 1);
	// Remove pane from layout
	void removePane(ViewPane* pane);
	// Return first pane in list
	ViewPane* panes();
	// Return last pane in list
	ViewPane* lastPane();
	// Return named pane (if it exists)
	ViewPane* pane(QString name);
	// Return index of specified pane in list
	int paneIndex(ViewPane* pane);
	// Return reflist of all panes of specified type
	RefList<ViewPane,bool> panes(ViewPane::PaneRole role);
	// Return reflist of panes (optionally of specified type) that target specified collection
	RefList<ViewPane,bool> panes(Collection* collection, ViewPane::PaneRole role = ViewPane::nPaneRoles);
	// Return whether collection is used anywhere on a pane (optionally only of specified type)
	ViewPane* collectionUsed(Collection* collection, ViewPane::PaneRole role = ViewPane::nPaneRoles);
	// Return if pane is in the current list
	bool containsPane(ViewPane* pane);
	// Return pane under specified coordinate
	ViewPane* paneAt(int layoutX, int layoutY);
	// Return pane containing specified grid reference
	ViewPane* paneAtGrid(int gridX, int gridY);
	// Translate pane by the amount specified
	void translatePane(ViewPane* pane, int deltaX, int deltaY);
	// Bring pane to front
	void bringPaneToFront(ViewPane* pane, bool onTop = false);
	// Send pane to back
	void sendPaneToBack(ViewPane* pane, bool onBottom = false);
	// Pane has changed
	void paneChanged(ViewPane* caller = NULL);
	// Reset view matrix of all panes
	void resetViewMatrix();
	// Update interaction primitives for all panes
	void updateInteractionPrimitives(int axis);
};

#endif
