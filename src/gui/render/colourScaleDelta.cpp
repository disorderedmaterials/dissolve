// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/colourScaleDelta.h"
#include "gui/render/colourScalePoint.h"

ColourScaleDelta::ColourScaleDelta()
{
    // Private variables
    start_ = 0.0;
    delta_ = 0.0;
    useHSV_ = false;
}

ColourScaleDelta::ColourScaleDelta(const ColourScalePoint &start, const ColourScalePoint &end, bool useHSV)
{
    set(start, end, useHSV);
}

// Check whether the delta 'contains' the supplied value
bool ColourScaleDelta::containsValue(double d) const
{
    if (d < start_)
        return false;
    if (d > (start_ + delta_))
        return false;
    return true;
}

// Create delta from ColourScalePoints
void ColourScaleDelta::set(const ColourScalePoint &start, const ColourScalePoint &end, bool useHSV)
{
    // Copy first colour point
    start_ = start.value();
    startColour_ = start.colour();
    useHSV_ = useHSV;

    // Set the colour delta
    if (useHSV)
    {
        deltaColourF_[0] = end.colour().hueF() - startColour_.hueF();
        deltaColourF_[1] = end.colour().saturationF() - startColour_.saturationF();
        deltaColourF_[2] = end.colour().valueF() - startColour_.valueF();
    }
    else
    {
        deltaColourF_[0] = end.colour().redF() - startColour_.redF();
        deltaColourF_[1] = end.colour().greenF() - startColour_.greenF();
        deltaColourF_[2] = end.colour().blueF() - startColour_.blueF();
    }
    deltaColourF_[3] = end.colour().alphaF() - startColour_.alphaF();

    delta_ = end.value() - start_;
}

// Return colour of the delta's starting point
QColor ColourScaleDelta::startColour() const { return startColour_; }

// Return colour of the delta's starting point as GLfloat*
void ColourScaleDelta::startColour(GLfloat *rgba) const
{
    rgba[0] = startColour_.redF();
    rgba[1] = startColour_.greenF();
    rgba[2] = startColour_.blueF();
    rgba[3] = startColour_.alphaF();
}

// Get colour for value v
QColor ColourScaleDelta::colour(double value) const
{
    // Clamp 'v' to range 0.0 - 1.0 to span range of delta
    double clampv = (value - start_) / delta_;
    if (clampv < 0.0)
        clampv = 0.0;
    else if (clampv > 1.0)
        clampv = 1.0;
    QColor col;
    if (useHSV_)
    {
        col.setHsvF(startColour_.hue() + deltaColourF_[0] * clampv, startColour_.saturationF() + deltaColourF_[1] * clampv,
                    startColour_.valueF() + deltaColourF_[2] * clampv);
    }
    else
    {
        col.setRedF(startColour_.redF() + deltaColourF_[0] * clampv);
        col.setGreenF(startColour_.greenF() + deltaColourF_[1] * clampv);
        col.setBlueF(startColour_.blueF() + deltaColourF_[2] * clampv);
    }

    col.setAlphaF(startColour_.alphaF() + deltaColourF_[3] * clampv);

    return col;
}

// Get colour for value as GLfloat* ranged from 0.0-1.0, assuming that v is within the range 0 -> value_
void ColourScaleDelta::colour(double v, GLfloat *rgba) const
{
    // Clamp 'v' to range 0.0 - 1.0 to span range of delta
    double clampv = (v - start_) / delta_;
    if (clampv < 0.0)
        clampv = 0.0;
    else if (clampv > 1.0)
        clampv = 1.0;
    if (useHSV_)
    {
        QColor col;
        col.setHsvF(startColour_.hueF() + deltaColourF_[0] * clampv, startColour_.saturationF() + deltaColourF_[1] * clampv,
                    startColour_.valueF() + deltaColourF_[2] * clampv);
        rgba[0] = col.redF();
        rgba[1] = col.greenF();
        rgba[2] = col.blueF();
    }
    else
    {
        rgba[0] = startColour_.redF() + deltaColourF_[0] * clampv;
        rgba[1] = startColour_.greenF() + deltaColourF_[1] * clampv;
        rgba[2] = startColour_.blueF() + deltaColourF_[2] * clampv;
    }
    rgba[3] = startColour_.alphaF() + deltaColourF_[3] * clampv;
}

// Return the starting value of the range
double ColourScaleDelta::start() const { return start_; }

// Return the range of the delta
double ColourScaleDelta::delta() const { return delta_; }
