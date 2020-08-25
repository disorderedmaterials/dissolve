/*
    *** Colour Scale
    *** src/gui/render/colourscale.cpp
    Copyright T. Youngs 2013-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/render/colourscale.h"

ColourScale::ColourScale()
{
    // Private variables
    interpolated_ = true;
    useHSV_ = false;
}

ColourScale::ColourScale(const ColourScale &source) { (*this) = source; }

void ColourScale::operator=(const ColourScale &source)
{
    clear();

    points_ = source.points_;
    useHSV_ = source.useHSV_;
    interpolated_ = source.interpolated_;

    calculateDeltas();
}

// Clear ColourScale
void ColourScale::clear()
{
    points_.clear();
    deltas_.clear();
}

/*
 * Points
 */

// Add point to scale
void ColourScale::addPoint(double value, QColor colour)
{
    /*
     * If supplied value is less than that at the start of the list, add it at the beginning.
     * If larget than the one at the end, then append it to the end of the list.
     * If neither of these, find the first existing value which is larger, and add it before that one.
     */
    if ((points_.nItems() == 0) || (value > points_.last().value()))
        points_.add(ColourScalePoint(value, colour));
    else
    {
        // Find a suitable insertion point now, so we don't have to reorder
        for (int n = 0; n < points_.nItems(); ++n)
        {
            if (points_[n].value() > value)
            {
                points_.insert(ColourScalePoint(value, colour), n);
                break;
            }
        }
    }

    // Recalculate colour deltas
    calculateDeltas();
}

// Return number of points in ColourScale
int ColourScale::nPoints() const { return points_.nItems(); }

// Return points in ColourScale
const Array<ColourScalePoint> &ColourScale::points() const { return points_; }

// Return first point in ColourScale
const ColourScalePoint &ColourScale::firstPoint() const { return points_.constAt(0); }

// Return last point in ColourScale
const ColourScalePoint &ColourScale::lastPoint() const { return points_.constAt(points_.nItems() - 1); }

// Return specific point in ColourScale
const ColourScalePoint &ColourScale::point(int id) const { return points_.constAt(id); }

// Set colour and value data for point
void ColourScale::setPoint(int position, double value, QColor colour)
{
    // Check position supplied
    if ((position < 0) || (position >= points_.nItems()))
    {
        Messenger::error("Scale point position to set ({}) is invalid - nItems = {}.\n", position, points_.nItems());
        return;
    }

    points_[position].setColour(colour);

    // Set the new value - this will also update the position of the point if necessary
    setValue(position, value);
}

// Set value for point
void ColourScale::setValue(int position, double value)
{
    // Store whether the new value is higher or lower than the existing one...
    auto newValueHigher = value >= points_[position].value();

    // Set the new value of the point
    points_[position].setValue(value);

    // Position in list may have changed - check...
    if (newValueHigher)
    {
        while ((position != (nPoints() - 1)) && value > points_[position + 1].value())
            points_.shiftUp(position++);
    }
    else
    {
        while ((position != 0) && value < points_[position - 1].value())
            points_.shiftDown(position--);
    }

    // Recalculate colour deltas
    calculateDeltas();
}

// Set colour for point
void ColourScale::setColour(int position, QColor colour)
{
    points_[position].setColour(colour);

    // Recalculate colour deltas
    calculateDeltas();
}

// Remove old point from ColourScale
void ColourScale::removePoint(int position)
{
    // Check position supplied
    if ((position < 0) || (position >= points_.nItems()))
    {
        Messenger::error("Scale point position to set ({}) is invalid - nItems = {}.\n", position, points_.nItems());
        return;
    }

    points_.remove(position);

    // Recalculate colour deltas
    calculateDeltas();
}

// Return colour associated with value provided
QColor ColourScale::colour(double value) const
{
    // Check for no points being defined
    if (points_.nItems() == 0)
        return QColor(0, 0, 0);

    // Is supplied value less than the value at the first point?
    if (value < points_.constAt(0).value())
        return points_.constAt(0).colour();
    else if (value > points_.constAt(nPoints() - 1).value())
        return points_.constAt(nPoints() - 1).colour();

    // Find the correct delta to use
    for (int n = 0; n < deltas_.nItems(); ++n)
    {
        if (deltas_.constAt(n).containsValue(value))
        {
            if (interpolated_)
                return deltas_.constAt(n).colour(value);
            else
                return deltas_.constAt(n).startColour();
        }
    }

    // Should never get here - all cases accounted for
    return QColor(0, 0, 0, 255);
}

// Get colour associated with value supplied, setting as GLfloat[4]
void ColourScale::colour(double value, GLfloat *rgba) const
{
    // Check for no points being defined
    if (points_.nItems() == 0)
    {
        rgba[0] = 0.0;
        rgba[1] = 0.0;
        rgba[2] = 0.0;
        rgba[3] = 1.0;
        return;
    }

    // Is supplied value less than the value at the first point?
    if (value < points_.constAt(0).value())
    {
        points_.constAt(0).colour(rgba);
        return;
    }
    else if (value > points_.constAt(nPoints() - 1).value())
    {
        points_.constAt(nPoints() - 1).colour(rgba);
        return;
    }

    // Find the correct delta to use
    for (int n = 0; n < deltas_.nItems(); ++n)
    {
        if (deltas_.constAt(n).containsValue(value))
        {
            if (interpolated_)
                return deltas_.constAt(n).colour(value, rgba);
            else
                return deltas_.constAt(n).startColour(rgba);
            return;
        }
    }

    // Shouldn't ever get here - TODO Raise Exception?
    rgba[0] = 0.0;
    rgba[1] = 0.0;
    rgba[2] = 0.0;
    rgba[3] = 1.0;
}

// Set all alpha values to that specified
void ColourScale::setAllAlpha(double alpha)
{
    auto alphai = alpha * 255;
    if (alphai < 0)
        alphai = 0;
    else if (alphai > 255)
        alphai = 255;

    for (int n = 0; n < points_.nItems(); ++n)
        points_[n].setAlpha(alphai);

    calculateDeltas();
}

/*
 * Deltas
 */

// Recalculate colour deltas between points
void ColourScale::calculateDeltas()
{
    // Reinitialise deltas array
    deltas_.clear();
    for (int n = 0; n < points_.nItems() - 1; ++n)
        deltas_.add(ColourScaleDelta(points_[n], points_[n + 1], useHSV_));
}

/*
 * Style
 */

// Set whether the ColourScale is interpolated
void ColourScale::setInterpolated(bool b) { interpolated_ = b; }

// Return whether the ColourScale is interpolated
bool ColourScale::interpolated() const { return interpolated_; }

// Set whether or not to use HSV interpolation
void ColourScale::setUseHSV(bool b)
{
    useHSV_ = b;

    calculateDeltas();
}

// Return whether or not to use HSV interpolation
bool ColourScale::useHSV() const { return useHSV_; }
