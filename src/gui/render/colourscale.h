// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/render/colourscaledelta.h"
#include "gui/render/colourscalepoint.h"
#include "templates/array.h"
#include "templates/vector4.h"
#include <QColor>
#include <QOpenGLFunctions>

// Forward declarations
/* none */

// Colour Scale
class ColourScale
{
    public:
    ColourScale();
    ColourScale(const ColourScale &);
    void operator=(const ColourScale &source);
    // Clear ColourScale
    void clear();

    /*
     * Points
     */
    private:
    // List of points in the ColourScale
    Array<ColourScalePoint> points_;

    public:
    // Add new point to ColourScale
    void addPoint(double value, QColor colour);
    // Return number of points in ColourScale
    int nPoints() const;
    // Return points in ColourScale
    const Array<ColourScalePoint> &points() const;
    // Return first point in ColourScale
    const ColourScalePoint &firstPoint() const;
    // Return last point in ColourScale
    const ColourScalePoint &lastPoint() const;
    // Return specific point in ColourScale
    const ColourScalePoint &point(int id) const;
    // Set colour and value data for point
    void setPoint(int position, double value, QColor colour);
    // Set value for specified point
    void setValue(int position, double value);
    // Set colour for specified point
    void setColour(int position, QColor colour);
    // Remove point from ColourScale
    void removePoint(int position);
    // Get colour associated with value supplied
    QColor colour(double value) const;
    // Get colour associated with value supplied, setting as GLfloat[4]
    void colour(double value, GLfloat *rgba) const;
    // Set all alpha values to that specified
    void setAllAlpha(double alpha);

    /*
     * Deltas
     */
    private:
    // List of colour deltas between points in the ColourScale
    Array<ColourScaleDelta> deltas_;

    private:
    // Calculate colour deltas for current list of points
    void calculateDeltas();

    /*
     * Style
     */
    private:
    // Whether colours are interpolated between points in the scale
    bool interpolated_;
    // Whether interpolation should be performed with HSV instead of RGB
    bool useHSV_;

    public:
    // Set whether the ColourScale is interpolated
    void setInterpolated(bool b);
    // Return whether the ColourScale is interpolated
    bool interpolated() const;
    // Set whether or not to use HSV interpolation
    void setUseHSV(bool b);
    // Return whether or not to use HSV interpolation
    bool useHSV() const;
};
