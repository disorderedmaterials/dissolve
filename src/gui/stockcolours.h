// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include <QColor>

// Forward Declarations
/* none */

// Stock Colours
class StockColours
{
    public:
    // Stock Colours
    enum StockColour
    {
        BlackStockColour,
        RedStockColour,
        GreenStockColour,
        BlueStockColour,
        PurpleStockColour,
        CyanStockColour,
        OrangeStockColour,
        PinkStockColour,
        LightRedStockColour,
        LightGreenStockColour,
        LightBlueStockColour,
        SilverStockColour,
        GoldStockColour,
        nStockColours
    };
    // Return enum options for AveragingScheme
    static EnumOptions<StockColours::StockColour> stockColours();
    // Return stock colour (as QColor)
    static QColor stockColour(StockColour colour);
    // Return stock colour (as QColor) given (wrapped) index supplied
    static QColor stockColourByIndex(int index);
};
