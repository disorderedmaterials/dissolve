/*
	*** Module Chart Metrics
	*** src/gui/modulechartmetrics.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_MODULECHARTMETRICS_H
#define DISSOLVE_MODULECHARTMETRICS_H

// Forward Declarations
/* none */

// Module Chart Metrics - Style / Layout Information
class ModuleChartMetrics
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
	// Border roudner radius
	static int blockBorderRadius()
	{
		return 6;
	}
	// Border mid-point
	static double blockBorderMidPoint()
	{
		return blockBorderWidth() / 2;
	}
	// Widget margins (to contain border line)
	static QMargins blockMargins()
	{
		int innerGap = 4;
		return QMargins(blockBorderWidth()+innerGap+blockDentRadius(), blockBorderWidth()+innerGap, blockBorderWidth()+innerGap, blockBorderWidth()+innerGap);
	}
	// Vertical offset at which dent begins on left-hand side
	static int blockDentOffset()
	{
		return 16;
	}
	// Radius of the dent
	static int blockDentRadius()
	{
		return 7;
	}
};

#endif
