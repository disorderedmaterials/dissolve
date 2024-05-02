// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/lineStyle.h"

LineStyle::LineStyle(double width, LineStipple::StippleType stipple, double r, double g, double b, double a)
{
    width_ = width;
    stipple_ = stipple;
    setColour(r, g, b, a);
}

LineStyle::~LineStyle() {}

LineStyle::LineStyle(const LineStyle &source) { (*this) = source; }

void LineStyle::operator=(const LineStyle &source)
{
    width_ = source.width_;
    stipple_ = source.stipple_;
    colour_[0] = source.colour_[0];
    colour_[1] = source.colour_[1];
    colour_[2] = source.colour_[2];
    colour_[3] = source.colour_[3];
}

/*
 * Style
 */

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple)
{
    width_ = width;
    stipple_ = stipple;
}

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, QColor colour)
{
    width_ = width;
    stipple_ = stipple;
    setColour(colour);
}

// Set line style
void LineStyle::set(double width, LineStipple::StippleType stipple, double r, double g, double b, double a)
{
    width_ = width;
    stipple_ = stipple;
    setColour(r, g, b, a);
}

// Set line width
void LineStyle::setWidth(double width) { width_ = width; }

// Return line width
double LineStyle::width() const { return width_; }

// Set line stipple
void LineStyle::setStipple(LineStipple::StippleType stipple) { stipple_ = stipple; }

// Return line stipple
LineStipple::StippleType LineStyle::stipple() const { return stipple_; }

// Set line colour
void LineStyle::setColour(double r, double g, double b, double a)
{
    colour_[0] = r;
    colour_[1] = g;
    colour_[2] = b;
    colour_[3] = a;
}

// Set line colour
void LineStyle::setColour(QColor colour)
{
    colour_[0] = colour.redF();
    colour_[1] = colour.greenF();
    colour_[2] = colour.blueF();
    colour_[3] = colour.alphaF();
}

// Return line colour
const GLfloat *LineStyle::colour() const { return colour_; }

/*
 * GL
 */

// Send line styling to GL
void LineStyle::sendToGL(const double pixelScaling) const
{
    // Set line width, including any supplied scaling factor
    glLineWidth(width_ * pixelScaling);

    // Enable / disable stippling
    if (stipple_ == LineStipple::NoStipple)
        glDisable(GL_LINE_STIPPLE);
    else
    {
        glEnable(GL_LINE_STIPPLE);
        LineStipple::stipple[stipple_].apply();
    }

    // Apply colour
    glColor4fv(colour_);
}
