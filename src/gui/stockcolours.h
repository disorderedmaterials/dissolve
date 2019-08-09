/*
	*** Stock Colours
	*** src/gui/stockcolours.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_STOCKCOLOURS_H
#define DISSOLVE_STOCKCOLOURS_H

#include "base/enumoptions.h"
#include <QColor>

// Forward Declarations
/* none */

// Stock Colours
class StockColours
{
	public:
	// Stock Colours
	enum StockColour { BlackStockColour, RedStockColour, GreenStockColour, BlueStockColour, PurpleStockColour, CyanStockColour, OrangeStockColour, PinkStockColour, LightRedStockColour, LightGreenStockColour, LightBlueStockColour, SilverStockColour, GoldStockColour, nStockColours };
	// Return enum options for AveragingScheme
	static EnumOptions<StockColours::StockColour> stockColours();
	// Return stock colour (as QColor)
	static QColor stockColour(StockColour colour);
};

#endif
