// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/stockcolours.h"

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
    static EnumOptionsList StockColoursOptions = EnumOptionsList()
                                                 << EnumOption(StockColours::BlackStockColour, "Black")
                                                 << EnumOption(StockColours::RedStockColour, "Red")
                                                 << EnumOption(StockColours::GreenStockColour, "Green")
                                                 << EnumOption(StockColours::BlueStockColour, "Blue")
                                                 << EnumOption(StockColours::PurpleStockColour, "Purple")
                                                 << EnumOption(StockColours::CyanStockColour, "Cyan")
                                                 << EnumOption(StockColours::OrangeStockColour, "Orange")
                                                 << EnumOption(StockColours::PinkStockColour, "Pink")
                                                 << EnumOption(StockColours::LightRedStockColour, "LightRed")
                                                 << EnumOption(StockColours::LightGreenStockColour, "LightGreen")
                                                 << EnumOption(StockColours::LightBlueStockColour, "LightBlue")
                                                 << EnumOption(StockColours::SilverStockColour, "Silver")
                                                 << EnumOption(StockColours::GoldStockColour, "Gold");

    static EnumOptions<StockColours::StockColour> options("StockColour", StockColoursOptions);

    return options;
}

// Return stock colour (as QColor)
QColor StockColours::stockColour(StockColours::StockColour colour) { return StockQColors[colour]; }

// Return stock colour (as QColor) given (wrapped) index supplied
QColor StockColours::stockColourByIndex(int index) { return StockQColors[index % stockColours().nOptions()]; }
