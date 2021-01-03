// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include <QPoint>
#include <QRect>

// Forward Declarations
class QWidget;

// Chart Block - Base class for any widget displayed in a chart
class ChartBlock : public ListItem<ChartBlock>
{
    public:
    ChartBlock();
    virtual ~ChartBlock();

    /*
     * Type
     */
    public:
    // Return type of this block
    virtual const QString blockType() const = 0;

    /*
     * Widget
     */
    private:
    // Proposed new geometry of the widget
    QRect newGeometry_;

    public:
    // Return underlying widget
    virtual QWidget *widget() = 0;
    // Return width of underlying widget
    virtual int widgetWidth() const = 0;
    // Return height of underlying widget
    virtual int widgetHeight() const = 0;
    // Set underlying widget position
    void setNewPosition(int left, int top);
    // Set right-hand side position of widget, adjusting its width
    void setNewRightEdge(int right);
    // Set underlying widget geometry
    void setNewSize(int width, int height);
    // Commit new geometry for widget
    void setNewGeometry(bool animate);
    // Return whether the supplied point (in global coordinates) allows a drag operation to begin
    virtual bool isDragPoint(QPoint globalPos) const = 0;

    /*
     * Update
     */
    public:
    // Update controls within widget
    virtual void updateControls() = 0;
    // Disable sensitive controls
    virtual void disableSensitiveControls() = 0;
    // Enable sensitive controls
    virtual void enableSensitiveControls() = 0;
};
