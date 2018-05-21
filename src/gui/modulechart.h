/*
	*** Module Chart
	*** src/gui/modulechart.h
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

#ifndef DUQ_MODULECHART_H
#define DUQ_MODULECHART_H

#include "templates/array2d.h"
#include "templates/reflist.h"
#include <QWidget>

// Forward Declarations
class DUQWindow;
class FlowBlock;
class FlowLayout;
class LineParser;
class Module;
class ModuleList;
class ModuleReference;

// Module Chart - Manages display of a sequence of Modules
class ModuleChart : public QWidget
{
	protected:
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Constructor / Destructor
	ModuleChart(DUQWindow* duqWindow, ModuleList& moduleList, QWidget* parent = NULL);
	virtual ~ModuleChart();

	private:
	// Pointer to DUQWindow
	DUQWindow* duqWindow_;


	/*
	 * QWidget Reimplementations
	 */
	public:
	// Geometry changed
	void resizeEvent(QResizeEvent* event);


	/*
	 * Display Widgets
	 */
	private:
	// Displayed Widgets
	RefList<FlowBlock,bool> displayedWidgets_;
	// Target ModuleList
	ModuleList& modules_;

	private:
	// Find FlowBlock displaying specified ModuleReference
	RefListItem<FlowBlock,bool>* flowBlockReference(ModuleReference* modRef);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();


	/*
	 * Widget Layout
	 */
	private:
	// Current number of columns of widgets
	int nColumns_;
	// Current number of rows of widgets
	int nRows_;
	// Current horizontal spacing between columns / edges
	int horizontalSpacing_;
	// Current vertical spacing between rows / edges
	int verticalSpacing_;
	// Left-hand-side coordinates of columns
	Array2D<int> lefts_;
	// Top-side coordinates of rows
	Array2D<int> tops_;

	private:
	// Lay out widgets snaking horizontally
	void layOutHorizontally();
	// Lay out widgets snaking vertically
	void layOutVertically();
	// Lay out the current widgets according to our available geometry
	void layOutWidgets();


	/*
	 * State I/O
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif
