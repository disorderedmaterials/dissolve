/*
	*** PlotDataStyle Functions
	*** src/gui/qcustomplot/plotdatastyle.cpp
	Copyright T. Youngs 2016-2017

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

#include "gui/qcustomplot/plotdatastyle.h"
#include <stdio.h>

DUQ_USING_NAMESPACE

/*
 * PlotDataStyle
 */

// External Declarations
PlotDataStyle PlotDataStyles::plotDataStyles_[] = {
	{ {255,0,0}, Qt::SolidLine, 0 },
	{ {0,255,0}, Qt::SolidLine, 0 },
	{ {0,0,255}, Qt::SolidLine, 0 },
	{ {255,0,0}, Qt::DashLine, 0 },
	{ {0,255,0}, Qt::DashLine, 0 },
	{ {0,0,255}, Qt::DashLine, 0 },
	{ {255,0,0}, Qt::DotLine, 0 },
	{ {0,255,0}, Qt::DotLine, 0 },
	{ {0,0,255}, Qt::DotLine, 0 }
};

/*
 * PlotDataStyles
 */

// Set pen and brush for the specified style
void PlotDataStyles::penAndBrush(PlotDataStyle::DataStyle style, QPen& pen, QBrush& brush)
{
	// If a proper style was provided, check that it hasn't already been used
	if (style != PlotDataStyle::nDataStyles)
	{
		// Check the useCount of the style
		if (plotDataStyles_[style].useCount > 0) printf("Warning - PlotDataStyle %i is already in use.\n", style);

		pen = QPen();
		pen.setColor(QColor(plotDataStyles_[style].colour[0], plotDataStyles_[style].colour[1], plotDataStyles_[style].colour[2], 255));
		pen.setStyle(plotDataStyles_[style].penStyle);

		brush = QBrush();
		brush.setColor(QColor(plotDataStyles_[style].colour[0], plotDataStyles_[style].colour[1], plotDataStyles_[style].colour[2], 128));
	}
	else
	{
		// No style specified, so use the next one that isn't already on the graph
	}
}

// Return use count for the specified style
int PlotDataStyles::useCount(PlotDataStyle::DataStyle style)
{
	if (style != PlotDataStyle::nDataStyles) return plotDataStyles_[style].useCount;
	else printf("Requested the useCount of a non-style.\n");

	return -1;
}
