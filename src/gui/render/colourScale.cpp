// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/colourScale.h"
#include <algorithm>

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
     * If larger than the one at the end, then append it to the end of the list.
     * If neither of these, find the first existing value which is larger, and add it before that one.
     */
    if (points_.empty() || (value > points_.back().value()))
        points_.emplace_back(value, colour);
    else
    {
        // Find a suitable insertion point
        auto it = std::find_if(points_.begin(), points_.end(), [value](const auto &point) { return point.value() > value; });
        points_.insert(it, {value, colour});
    }

    // Recalculate colour deltas
    calculateDeltas();
}

// Return number of points in ColourScale
int ColourScale::nPoints() const { return points_.size(); }

// Return points in ColourScale
const std::vector<ColourScalePoint> &ColourScale::points() const { return points_; }

// Return first point in ColourScale
const ColourScalePoint &ColourScale::firstPoint() const { return points_.front(); }

// Return last point in ColourScale
const ColourScalePoint &ColourScale::lastPoint() const { return points_.back(); }

// Return specific point in ColourScale
const ColourScalePoint &ColourScale::point(int id) const { return points_[id]; }

// Set colour and value data for point
void ColourScale::setPoint(int position, double value, QColor colour)
{
    assert(position >= 0 && position < points_.size());

    points_[position].setColour(colour);

    // Set the new value - this will also update the position of the point if necessary
    setValue(position, value);
}

// Set value for point
void ColourScale::setValue(int position, double value)
{
    // Set the new value of the point
    points_[position].setValue(value);

    // Position in list may have changed...
    std::sort(points_.begin(), points_.end(), [](const auto &p1, const auto &p2) { return p1.value() < p2.value(); });

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
    assert(position >= 0 && position < points_.size());

    points_.erase(points_.begin() + position);

    // Recalculate colour deltas
    calculateDeltas();
}

// Return colour associated with value provided
QColor ColourScale::colour(double value) const
{
    // Check for no points being defined
    if (points_.empty())
        return QColor(0, 0, 0);

    // Is supplied value less than the value at the first point?
    if (value <= points_.front().value())
        return points_.front().colour();
    else if (value >= points_.back().value())
        return points_.back().colour();

    // Find the correct delta to use
    auto it = std::find_if(deltas_.begin(), deltas_.end(), [value](const auto &delta) { return delta.containsValue(value); });
    if (it != deltas_.end())
        return interpolated_ ? it->colour(value) : it->startColour();

    throw(std::runtime_error(fmt::format("Failed to find a colour to return from the ColourScale.\n")));
}

// Get colour associated with value supplied, setting as GLfloat[4]
void ColourScale::colour(double value, GLfloat *rgba) const
{
    auto qcol = colour(value);

    rgba[0] = qcol.redF();
    rgba[1] = qcol.greenF();
    rgba[2] = qcol.blueF();
    rgba[3] = qcol.alphaF();
}

/*
 * Deltas
 */

// Recalculate colour deltas between points
void ColourScale::calculateDeltas()
{
    deltas_.clear();
    if (points_.empty())
        return;

    for (auto n = points_.begin(); std::next(n) != points_.end(); ++n)
        deltas_.emplace_back(*n, *std::next(n), useHSV_);
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
