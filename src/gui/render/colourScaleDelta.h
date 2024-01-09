// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
class ColourScalePoint;

// Colour Scale Delta
class ColourScaleDelta
{
    public:
    ColourScaleDelta();
    ColourScaleDelta(const ColourScalePoint &start, const ColourScalePoint &end, bool useHSV);

    private:
    // Value at which the delta starts
    double start_;
    // Range of the data from the start value
    double delta_;
    // Colour of the delta's starting point
    QColor startColour_;
    // Delta between the starting point and the final point
    float deltaColourF_[4];
    // Whether to use HSV, rather than RGB, when generating colours
    bool useHSV_;

    public:
    // Check whether the delta 'contains' the supplied value
    bool containsValue(double d) const;
    // Create delta from ColourScalePoints
    void set(const ColourScalePoint &start, const ColourScalePoint &end, bool useHSV);
    // Return colour of the delta's starting point
    QColor startColour() const;
    // Return colour of the delta's starting point as GLfloat[4]
    void startColour(GLfloat *rgba) const;
    // Get colour for value, assuming that v is within the range 0 -> value_
    QColor colour(double v) const;
    // Get colour for value as GLfloat[4], assuming that v is within the range 0 -> value_
    void colour(double v, GLfloat *rgba) const;
    // Return the starting value of the range
    double start() const;
    // Return the range of the delta
    double delta() const;
};
