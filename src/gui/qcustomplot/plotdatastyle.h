/*
	*** Plot Data Style
	*** src/gui/qcustomplot/plotdatastyle.h
	Copyright T. Youngs 2007-2017

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

#ifndef DUQ_PLOTDATASTYLE_H
#define DUQ_PLOTDATASTYLE_H

#include <QPen>

// Style information for plotted data
class PlotDataStyle
{
	public:
	// Styles
	enum DataStyle { RedSolidStyle, GreenSolidStyle, BlueSolidStyle, RedDashStyle, GreenDashStyle, BlueDashStyle, RedDotStyle, GreenDotStyle, BlueDotStyle, nDataStyles };


	/*
	 * Style Data
	 */
	public:
	// Basic colour
	int colour[3];
	// Qt pen style
	Qt::PenStyle penStyle;
	// Whether this style is currently in use
	int useCount;
};

// Access class for PlotDataStyles
class PlotDataStyles
{
	private:
	// Available styles
	static PlotDataStyle plotDataStyles_[];


	/*
	 * Style Access
	 */
	public:
	// Set pen and brush for the specified style
	static void penAndBrush(PlotDataStyle::DataStyle style, QPen& pen, QBrush& brush);
	// Return use count for the specified style
	static int useCount(PlotDataStyle::DataStyle style);
};

#endif

