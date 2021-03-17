// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/charts/charthotspot.h"
#include <stdio.h>

ChartHotSpot::ChartHotSpot() : ListItem<ChartHotSpot>()
{
    row_ = -1;
    column_ = -1;
    type_ = ChartHotSpot::nHotSpotTypes;
    blockAfter_ = nullptr;
    blockBefore_ = nullptr;
}

ChartHotSpot::~ChartHotSpot() {}

/*
 * Definition
 */

// Set type of the hotspot
void ChartHotSpot::setType(ChartHotSpot::HotSpotType type) { type_ = type; }

// Return the hotspot type
ChartHotSpot::HotSpotType ChartHotSpot::type() const { return type_; }

// Set location information
void ChartHotSpot::setLocation(int row, int column)
{
    row_ = row;
    column_ = column;
}

// Return row in which the hotspot exists
int ChartHotSpot::row() const { return row_; }

// Return column in which the hotspot exists
int ChartHotSpot::column() const { return column_; }

// Set area information
void ChartHotSpot::setGeometry(QRect geometry) { geometry_ = geometry; }

// Set the width of the hotspot
void ChartHotSpot::setWidth(int width) { geometry_.setWidth(width); }

// Set the height of the hotspot
void ChartHotSpot::setHeight(int height) { geometry_.setHeight(height); }

// Return the geometry of the area
QRect ChartHotSpot::geometry() const { return geometry_; }

// Return if the supplied point exists within the hotspot
bool ChartHotSpot::contains(QPoint p) const { return geometry_.contains(p); }

// Set blocks urrounding the hotspot
void ChartHotSpot::setSurroundingBlocks(ChartBlock *blockBefore, ChartBlock *blockAfter)
{
    blockBefore_ = blockBefore;
    blockAfter_ = blockAfter;
}

// Return pointer to ChartBlock immediately before the hotspot
ChartBlock *ChartHotSpot::blockBefore() const { return blockBefore_; }

// Return pointer to ChartBlock immediately after the hotspot
ChartBlock *ChartHotSpot::blockAfter() const { return blockAfter_; }
