/*
	*** Stock Colours
	*** src/gui/stockcolours.cpp
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

#include "gui/stockcolours.h"

// Stock Colours
QColor StockQColors[] = {
	QColor(0, 0, 0),	/* BlackStockColour */
	QColor(200, 0, 0),	/* RedStockColour */
	QColor(0, 200, 0),	/* GreenStockColour */
	QColor(0, 0, 200),	/* BlueStockColour */
	QColor(200, 0, 200),	/* PurpleStockColour */
	QColor(0, 200, 200),	/* CyanStockColour */
	QColor(255, 128, 0),	/* OrangeStockColour */
	QColor(255, 100, 255),	/* PinkStockColour */
	QColor(255, 100, 100),	/* LightRedStockColour */
	QColor(100, 255, 100),	/* LightGreenStockColour */
	QColor(100, 100, 255),	/* LightBlueStockColour */
	QColor(128, 128, 128),	/* SilverStockColour */
	QColor(128, 128, 50)	/* GoldStockColour */
};

// Return enum options for RenderableGroupKeyword
EnumOptions<StockColours::StockColour> StockColours::stockColours()
{
	static EnumOptionsList StockColoursOptions = EnumOptionsList() << 
		EnumOption(StockColours::BlackStockColour,		"Black" ) <<
		EnumOption(StockColours::RedStockColour,		"Red" ) <<
		EnumOption(StockColours::GreenStockColour,		"Green" ) <<
		EnumOption(StockColours::BlueStockColour,		"Blue" ) <<
		EnumOption(StockColours::PurpleStockColour,		"Purple" ) <<
		EnumOption(StockColours::CyanStockColour,		"Cyan" ) <<
		EnumOption(StockColours::OrangeStockColour,		"Orange" ) <<
		EnumOption(StockColours::PinkStockColour,		"Pink" ) <<
		EnumOption(StockColours::LightRedStockColour,		"LightRed" ) <<
		EnumOption(StockColours::LightGreenStockColour,		"LightGreen" ) <<
		EnumOption(StockColours::LightBlueStockColour,		"LightBlue" ) <<
		EnumOption(StockColours::SilverStockColour,		"Silver" ) <<
		EnumOption(StockColours::GoldStockColour,		"Gold" );

	static EnumOptions<StockColours::StockColour> options("StockColour", StockColoursOptions);

	return options;
}

// Return stock colour (as QColor)
QColor StockColours::stockColour(StockColours::StockColour colour)
{
	return StockQColors[colour];
}

// Return stock colour (as QColor) given (wrapped) index supplied
QColor StockColours::stockColourByIndex(int index)
{
	return StockQColors[index%stockColours().nOptions()];
}
