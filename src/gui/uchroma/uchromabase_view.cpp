/*
	*** UChroma Base - View
	*** src/gui/uchroma/uchromabase_view.cpp
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

#include "gui/uchroma/uchromabase.h"

// Return curreent view layout
ViewLayout& UChromaBase::viewLayout()
{
	return viewLayout_;
}

// Set view layout
void UChromaBase::setViewLayout(ViewLayout& source)
{
	viewLayout_ = source;
	currentViewPane_ = viewLayout_.panes();
}

// Recalculate layout pane positions / sizes (after context resize etc.)
void UChromaBase::recalculateViewLayout(int contextWidth, int contextHeight)
{
	viewLayout_.recalculate(contextWidth, contextHeight);

	updateDisplay();
}

// Set current view pane to the one under the specified screen coordinates
bool UChromaBase::setCurrentViewPane(int layoutX, int layoutY)
{
	ViewPane* newCurrentPane = viewLayout_.paneAt(layoutX, layoutY);
	if (newCurrentPane == NULL) return false;

	currentViewPane_ = newCurrentPane;

	// Toolbars and subwindows now need updating
	updateToolBars();

	return true;
}

// Set current view pane
bool UChromaBase::setCurrentViewPane(ViewPane* viewPane)
{
	currentViewPane_ = viewPane;

	// Toolbars and subwindows now need updating
	updateToolBars();

	return true;
}

// Return current view pane
ViewPane* UChromaBase::currentViewPane()
{
	return currentViewPane_;
}
