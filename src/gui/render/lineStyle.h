// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/render/lineStipple.h"
#include <QColor>

// Line Style
class LineStyle
{
    public:
    LineStyle(double width = 1.0, LineStipple::StippleType stipple = LineStipple::NoStipple, double r = 0.0, double g = 0.0,
              double b = 0.0, double a = 1.0);
    ~LineStyle();
    LineStyle(const LineStyle &source);
    void operator=(const LineStyle &source);

    /*
     * Style
     */
    private:
    // Line width
    double width_;
    // Line stipple
    LineStipple::StippleType stipple_;
    // Line colour
    GLfloat colour_[4];

    public:
    // Set line style
    void set(double width, LineStipple::StippleType stipple);
    // Set line style and colour
    void set(double width, LineStipple::StippleType stipple, double r, double g, double b, double a = 1.0);
    // Set line style and colour
    void set(double width, LineStipple::StippleType stipple, QColor colour);
    // Set line width
    void setWidth(double width);
    // Return line width
    double width() const;
    // Set line stipple
    void setStipple(LineStipple::StippleType stipple);
    // Return line stipple
    LineStipple::StippleType stipple() const;
    // Set line colour
    void setColour(double r, double g, double b, double a = 1.0);
    // Set line colour
    void setColour(QColor colour);
    // Return line colour
    const GLfloat *colour() const;

    /*
     * GL
     */
    public:
    // Send line styling to GL
    void sendToGL(const double pixelScaling = 1.0) const;
};
