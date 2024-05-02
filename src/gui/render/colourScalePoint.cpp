// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/colourScalePoint.h"

ColourScalePoint::ColourScalePoint(double value, QColor colour)
{
    // Private variables
    value_ = value;
    setColour(colour);
}
ColourScalePoint::~ColourScalePoint() {}

ColourScalePoint::ColourScalePoint(const ColourScalePoint &source) { (*this) = source; }

void ColourScalePoint::operator=(const ColourScalePoint &source)
{
    value_ = source.value_;
    colour_ = source.colour_;
}

/*
 * Value and Colour
 */

// Set value and colour for point
void ColourScalePoint::set(double value, QColor colour)
{
    value_ = value;
    colour_ = colour;
}

// Set value of point
void ColourScalePoint::setValue(double d) { value_ = d; }

// Return value of point
double ColourScalePoint::value() const { return value_; }

// Set colour
void ColourScalePoint::setColour(QColor colour) { colour_ = colour; }

// Return colour
QColor ColourScalePoint::colour() const { return colour_; }

// Return colour as GLfloat*
void ColourScalePoint::colour(GLfloat *rgba) const
{
    rgba[0] = colour_.redF();
    rgba[1] = colour_.greenF();
    rgba[2] = colour_.blueF();
    rgba[3] = colour_.alphaF();
}

// Set alpha value
void ColourScalePoint::setAlpha(int alpha) { colour_.setAlpha(alpha); }
