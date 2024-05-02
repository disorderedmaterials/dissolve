// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/stockColours.h"

// Stock Colours
QColor StockQColors[] = {
    QColor(0, 0, 0),       /* BlackStockColour */
    QColor(200, 0, 0),     /* RedStockColour */
    QColor(0, 200, 0),     /* GreenStockColour */
    QColor(0, 0, 200),     /* BlueStockColour */
    QColor(200, 0, 200),   /* PurpleStockColour */
    QColor(0, 200, 200),   /* CyanStockColour */
    QColor(255, 128, 0),   /* OrangeStockColour */
    QColor(255, 100, 255), /* PinkStockColour */
    QColor(255, 100, 100), /* LightRedStockColour */
    QColor(100, 255, 100), /* LightGreenStockColour */
    QColor(100, 100, 255), /* LightBlueStockColour */
    QColor(128, 128, 128), /* SilverStockColour */
    QColor(128, 128, 50)   /* GoldStockColour */
};

// Return enum options for RenderableGroupKeyword
EnumOptions<StockColours::StockColour> StockColours::stockColours()
{
    return EnumOptions<StockColours::StockColour>("StockColour", {{StockColours::BlackStockColour, "Black"},
                                                                  {StockColours::RedStockColour, "Red"},
                                                                  {StockColours::GreenStockColour, "Green"},
                                                                  {StockColours::BlueStockColour, "Blue"},
                                                                  {StockColours::PurpleStockColour, "Purple"},
                                                                  {StockColours::CyanStockColour, "Cyan"},
                                                                  {StockColours::OrangeStockColour, "Orange"},
                                                                  {StockColours::PinkStockColour, "Pink"},
                                                                  {StockColours::LightRedStockColour, "LightRed"},
                                                                  {StockColours::LightGreenStockColour, "LightGreen"},
                                                                  {StockColours::LightBlueStockColour, "LightBlue"},
                                                                  {StockColours::SilverStockColour, "Silver"},
                                                                  {StockColours::GoldStockColour, "Gold"}});
}

// Return stock colour (as QColor)
QColor StockColours::stockColour(StockColours::StockColour colour) { return StockQColors[colour]; }

// Return stock colour (as QColor) given (wrapped) index supplied
QColor StockColours::stockColourByIndex(int index) { return StockQColors[index % stockColours().nOptions()]; }
