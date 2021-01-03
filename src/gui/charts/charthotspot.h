// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <QRect>

// Forward Declarations
class ChartBlock;

// Chart HotSpot - Definition of an area onto which a dragged object can be dropped
class ChartHotSpot : public ListItem<ChartHotSpot>
{
    public:
    ChartHotSpot();
    ~ChartHotSpot();
    // HotSpot type
    enum HotSpotType
    {
        InsertionHotSpot,
        AppendHotSpot,
        DeleteHotSpot,
        nHotSpotTypes
    };

    /*
     * Definition
     */
    private:
    // Hotspot type
    HotSpotType type_;
    // Row of the parent layout in which the hotspot exists
    int row_;
    // Column of the parent layout in which the hotspot exists
    int column_;
    // Hotspot geometry
    QRect geometry_;
    // ChartBlock immediately before the hot spot
    ChartBlock *blockBefore_;
    // ChartBlock immediately after the hot spot
    ChartBlock *blockAfter_;

    public:
    // Set type of the hotspot
    void setType(HotSpotType type);
    // Return the hotspot type
    HotSpotType type() const;
    // Set location information
    void setLocation(int row, int column);
    // Return row in which the hotspot exists
    int row() const;
    // Return column in which the hotspot exists
    int column() const;
    // Set area information
    void setGeometry(QRect geometry);
    // Set the width of the hotspot
    void setWidth(int width);
    // Set the height of the hotspot
    void setHeight(int height);
    // Return the geometry of the area
    QRect geometry() const;
    // Return if the supplied point exists within the hotspot
    bool contains(QPoint p) const;
    // Set blocks urrounding the hotspot
    void setSurroundingBlocks(ChartBlock *blockBefore, ChartBlock *blockAfter);
    // Return pointer to ChartBlock immediately before the hotspot
    ChartBlock *blockBefore() const;
    // Return pointer to ChartBlock immediately after the hotspot
    ChartBlock *blockAfter() const;
};
