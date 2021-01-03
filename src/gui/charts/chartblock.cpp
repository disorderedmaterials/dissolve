// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/charts/chartblock.h"
#include <QPropertyAnimation>
#include <QWidget>

ChartBlock::ChartBlock() : ListItem<ChartBlock>() {}

ChartBlock::~ChartBlock() {}

/*
 * Widget
 */

// Set underlying widget position
void ChartBlock::setNewPosition(int left, int top)
{
    newGeometry_.setLeft(left);
    newGeometry_.setTop(top);
}

// Set right-hand side position of widget, adjusting its width
void ChartBlock::setNewRightEdge(int right) { newGeometry_.setWidth(right - newGeometry_.left()); }

// Set underlying widget geometry
void ChartBlock::setNewSize(int width, int height)
{
    newGeometry_.setWidth(width);
    newGeometry_.setHeight(height);
}

// Commit new geometry for widget
void ChartBlock::setNewGeometry(bool animate)
{
    if (animate)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(widget(), "geometry");
        animation->setDuration(100);
        animation->setEndValue(newGeometry_);
        animation->start();
    }
    else
        widget()->setGeometry(newGeometry_);
}
