/*
	*** Module Chart
	*** src/gui/modulechart.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/modulechart.h"
#include "gui/flowblock.h"
#include "module/list.h"
#include "module/module.h"

// Constructor
ModuleChart::ModuleChart(DUQWindow* duqWindow, ModuleList& modules, QWidget* parent) : QWidget(parent), modules_(modules)
{
	duqWindow_ = duqWindow;

	nColumns_ = 1;
	nRows_ = 0;
	horizontalSpacing_ = 0;
	verticalSpacing_ = 0;

	refreshing_ = false;

	updateControls();
}

ModuleChart::~ModuleChart()
{
}

/*
 * QWidget Reimplementations
 */

// Geometry changed
void ModuleChart::resizeEvent(QResizeEvent* event)
{
	layOutWidgets();
}

/*
 * Display Widgets
 */

// Find FlowBlock displaying specified ModuleReference
RefListItem<FlowBlock,bool>* ModuleChart::flowBlockReference(ModuleReference* modRef)
{
	RefListIterator<FlowBlock,bool> flowBlockIterator(displayedWidgets_);
	while (FlowBlock* block = flowBlockIterator.iterate()) if (block->moduleReference() == modRef) return flowBlockIterator.currentItem();

	return NULL;
}

// Update controls within widget
void ModuleChart::updateControls()
{
	// Step through the Modules in the list, we'll construct a new RefList of displayed widgets in the process.
	RefList<FlowBlock,bool> newDisplayedWidgets;
	for (ModuleReference* modRef = modules_.modules().first(); modRef != NULL; modRef = modRef->next)
	{
		// For this ModuleReference, does a current FlowBlock match?
		RefListItem<FlowBlock,bool>* blockRef = flowBlockReference(modRef);
		if (blockRef)
		{
			// Widget already exists, so remove the reference from the old list and add it to our new one
			newDisplayedWidgets.add(blockRef->item, blockRef->data);
			displayedWidgets_.remove(blockRef);
		}
		else
		{
			// No current FlowBlock reference, so must create suitable widget
			FlowBlock* flowBlock = new FlowBlock(this, duqWindow_, modRef);
			newDisplayedWidgets.add(flowBlock);
		}
	}

	// Any items remaining in the displayedWidgets_ list must now be deleted? TODO
// 	RefListIterator<FlowBlock,bool> removalIterator(displayedWidgets_);
// 	while (FlowBlock* block = removalIterator.iterate()) 

	// Copy the new RefList
	displayedWidgets_ = newDisplayedWidgets;

	// Update the layout
	layOutWidgets();
}

// Disable sensitive controls within widget, ready for main code to run
void ModuleChart::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void ModuleChart::enableSensitiveControls()
{
}

/*
 * Widget Layout
*/

// Lay out widgets snaking horizontally
void ModuleChart::layOutHorizontally()
{
	/*
	 * Determine how many columns we can fit across our current geometry, obeying a minimum spacing between widgets.
	 * Start by trying to lay out everything on one line. If this doesn't fit, increase the number of rows and try again.
	 */
	const int minSpacing = 32;
	const int maxWidth = width() - 2*minSpacing;
	int colCount;
	QList<int> columnWidths;
	int totalColumnWidth, lastNCols = 0, maxColumns = displayedWidgets_.nItems();

	for (nColumns_ = displayedWidgets_.nItems(); nColumns_ > 1; --nColumns_)
	{
		/*
		 * Loop over FlowBlocks, summing their widths as we go for each row.
		 * Also keep track of total column width over all rows - if we exceed this at any point we must continue the outer loop (reduce nColumns_).
		 */
		nRows_ = 1;
		colCount = 0;
		totalColumnWidth = 0;
		columnWidths.clear();
		RefListIterator<FlowBlock,bool> blockIterator(displayedWidgets_);
		while (FlowBlock* block = blockIterator.iterate())
		{
			// Get the width of this block
			int blockWidth = block->width();

			// If this is the first row, add our width to the columnWidths array, and increase totalColumnWidth
			if (nRows_ == 1)
			{
				columnWidths += blockWidth;
				totalColumnWidth += blockWidth;
				if (!blockIterator.first()) totalColumnWidth += minSpacing;
			}
			else
			{
				// Not the first row, so check this width against the one stored for the current column.
				if (blockWidth > columnWidths[colCount])
				{
					// The current widget is wider than any other in this column. Need to check the totalColumnWidth taking account of the difference
					totalColumnWidth += (blockWidth - columnWidths[colCount]);
					columnWidths[colCount] = blockWidth;
				}
			}

			// Check for failure...
			if (totalColumnWidth > maxWidth) break;

			// Added this widget OK - if it was the last one in the list, we have found a suitable number of columns
			if (blockIterator.last()) break;
			
			// Not the end of the list, so increase column count and check against the current nColumns_
			++colCount;
			if (colCount == nColumns_)
			{
				colCount = 0;
				++nRows_;
			}
		}

		// If we get to here and we haven't failed, we must have succeeded!
		if (totalColumnWidth <= maxWidth) break;
	}

	// Now have new number of columns and rows
// 	printf("NCOLS = %i, nROWS = %i\n", nColumns_, nRows_);

	// Work out row / column top-lefts
	tops_.initialise(nRows_, nColumns_);
	lefts_.initialise(nRows_, nColumns_);
	RefListIterator<FlowBlock,bool> blockIterator(displayedWidgets_);
	int top = minSpacing, rowMaxHeight;
	for (int row = 0; row < nRows_; ++row)
	{
		int left = minSpacing;
		rowMaxHeight = 0;
		for (int col = 0; col < nColumns_; ++col)
		{
			// Get FlowBlock for this grid reference (i.e. the next one in the list)
			FlowBlock* block = blockIterator.iterate();

			// Check the current height against the required height for this block's widget (if we still have a current one) and update if necessary
			if (block && (block->height() > rowMaxHeight)) rowMaxHeight = block->height();

			// Store this position, and set the widgets top-left
			tops_.ref(row, col) = top;
			lefts_.ref(row, col) = left;
			if (block) block->move(left, top);

			// Increase left-hand coordinate
			left += columnWidths.at(col) + minSpacing;
		}

		// Increase top-side coordinate
		top += rowMaxHeight + minSpacing;
	}
}

// Lay out widgets snaking vertically
void ModuleChart::layOutVertically()
{
}

// Lay out the current widgets according to our available geometry
void ModuleChart::layOutWidgets()
{
	layOutHorizontally();
}
