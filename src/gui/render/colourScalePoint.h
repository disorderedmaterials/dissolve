// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Colour Scale Point
class ColourScalePoint
{
    public:
    ColourScalePoint(double value = 0.0, QColor colour = QColor());
    ~ColourScalePoint();
    ColourScalePoint(const ColourScalePoint &source);
    void operator=(const ColourScalePoint &source);

    /*
     * Value and Colour
     */
    private:
    // Value at which this point occurs
    double value_;
    // Colour of this point
    QColor colour_;

    public:
    // Set value and colour for point
    void set(double value, QColor colour);
    // Set value for point
    void setValue(double value);
    // Return value for point
    double value() const;
    // Set colour
    void setColour(QColor colour);
    // Return colour
    QColor colour() const;
    // Return colour as GLfloat[4]
    void colour(GLfloat *rgba) const;
    // Set alpha value
    void setAlpha(int alpha);
};
