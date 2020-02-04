/*
	*** Procedure Chart Metrics
	*** src/gui/charts/proceduremetrics.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_CHARTS_PROCEDUREMETRICS_H
#define DISSOLVE_CHARTS_PROCEDUREMETRICS_H

#include <QMargins>

// Forward Declarations
/* none */

// Procedure Chart Metrics - Style / Layout Information
class ProcedureChartMetrics
{
	/*
	 * Module Chart Block
	 */
	public:
	// Border line width
	static int blockBorderWidth()
	{
		return 2;
	}
	// Border mid-point
	static double blockBorderMidPoint()
	{
		return (blockBorderWidth() / 2.0);
	}
	// Widget margins (to contain border line)
	static QMargins blockMargins()
	{
		int innerGap = 4;
		return QMargins(blockBorderWidth()+innerGap+dragHandleWidth(), blockBorderWidth()+innerGap, blockBorderWidth()+innerGap, blockBorderWidth()+innerGap);
	}
	// Vertical spacing between blocks
	static int blockVerticalSpacing()
	{
		return 2;
	}
	// Indent width
	static int indentWidth()
	{
		return 30;
	}
	// Drag handle width
	static int dragHandleWidth()
	{
		return 10;
	}


	/*
	 * Procedure Chart
	 */
	public:
	// Inner margin around widgets
	static int chartMargin()
	{
		return 0;
	}
	// Spacing between rows
	static int chartRowSpacing()
	{
		return 0;
	}
};

#endif
