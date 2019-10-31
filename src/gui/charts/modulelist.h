/*
	*** ModuleList Chart
	*** src/gui/charts/modulelist.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_CHARTS_MODULELIST_H
#define DISSOLVE_CHARTS_MODULELIST_H

#include "gui/charts/chart.hui"
#include "gui/charts/modulelistmetrics.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class Module;
class ModuleBlock;
class ModuleList;

// ModuleList Chart - Manages display of a sequence of modules from a ModuleList
class ModuleListChart : public ChartBase
{
	public:
	// Constructor / Destructor
	ModuleListChart(ModuleList* moduleList, Dissolve& dissolve);
	~ModuleListChart();


	/*
	 * Dissolve Reference
	 */
	private:
	// Dissolve reference
	Dissolve& dissolve_;


	/*
	 * Target ModuleList
	 */
	private:
	// Target ModuleList for display
	ModuleList* moduleList_;


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Chart Blocks
	 */
	private:
	// Widgets for our Module blocks
	RefList<ModuleBlock> moduleBlockWidgets_;

	private:
	// Find ModuleBlock displaying specified Module
	ModuleBlock* moduleBlock(Module* module);

	protected:
	// Update the content block widgets against the current target data
	void updateContentBlocks();


	/*
	 * Drag / Drop
	 */
	protected:
	// TODO


	/*
	 * Widget Layout
	 */
	private:
	// Chart metrics
	ModuleListChartMetrics metrics_;

	private:
	// Calculate new widget geometry according to the layout requirements, returning the entire area required
	QSize calculateNewWidgetGeometry(QSize currentSize);


	/*
	 * State I/O (ChartBase Reimplementations)
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif
