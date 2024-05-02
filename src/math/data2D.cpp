// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/data2D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "math/data1D.h"
#include "math/histogram2D.h"
#include "templates/arrayIndex2D.h"
#include <algorithm>
#include <cassert>

Data2D::Data2D() {}

Data2D::Data2D(const Data2D &source) { (*this) = source; }

// Clear Data
void Data2D::clear()
{
    x_.clear();
    y_.clear();
    values_.clear();
    errors_.clear();

    ++version_;
}

/*
 * Data
 */

// Set tag
void Data2D::setTag(std::string_view tag) { tag_ = tag; }

// Return tag
std::string_view Data2D::tag() const { return tag_; }

// Initialise arrays to specified size
void Data2D::initialise(int xSize, int ySize, bool withError)
{
    x_.clear();
    x_.resize(xSize);
    y_.clear();
    y_.resize(ySize);
    values_.initialise(xSize, ySize);
    hasError_ = withError;
    if (hasError_)
        errors_.initialise(xSize, ySize);
    else
        errors_.clear();

    ++version_;
}

// Initialise to be consistent in size and axes with supplied object
void Data2D::initialise(const Data2D &source)
{
    x_ = source.x_;
    y_ = source.y_;
    values_.initialise(x_.size(), y_.size());
    hasError_ = source.hasError_;
    if (hasError_)
        errors_.initialise(x_.size(), y_.size());
    else
        errors_.clear();

    ++version_;
}

// Initialise from supplied axis ranges
void Data2D::initialise(double xMin, double xMax, double xBin, double yMin, double yMax, double yBin, bool withError)
{
    auto xRange = xMax - xMin;
    auto yRange = yMax - yMin;
    int nXBins = xRange / xBin;
    int nYBins = yRange / yBin;

    // We will clamp the maxima to the nearest bin boundary (not less than the supplied axis maxima)
    if ((xMin + nXBins * xBin) < xMax)
        ++nXBins;
    if ((yMin + nYBins * yBin) < yMax)
        ++nYBins;

    // Create x_ axis array
    x_.clear();
    x_.resize(nXBins);
    auto xCentre = xMin + xBin * 0.5;
    for (auto n = 0; n < nXBins; ++n, xCentre += xBin)
        x_[n] = xCentre;

    // Create y_ axis array
    y_.clear();
    y_.resize(nXBins);
    auto yCentre = yMin + yBin * 0.5;
    for (auto n = 0; n < nYBins; ++n, yCentre += yBin)
        y_[n] = yCentre;

    // Initialise values array
    values_.initialise(nXBins, nYBins);

    // Initialise errors array if required
    hasError_ = withError;
    if (hasError_)
        errors_.initialise(nXBins, nYBins);
    else
        errors_.clear();

    ++version_;
}

// Copy arrays from supplied object
void Data2D::copyArrays(const Data2D &source)
{
    x_ = source.x_;
    y_ = source.y_;
    values_ = source.values_;
    errors_ = source.errors_;
    hasError_ = source.hasError_;

    ++version_;
}

// Zero values array
void Data2D::zero()
{
    values_ = 0.0;
    if (hasError_)
        errors_ = 0.0;

    ++version_;
}

// Return data version
int Data2D::version() const { return version_; }

// Return x value specified
double &Data2D::xAxis(int index)
{
    ++version_;

    return x_[index];
}

const double &Data2D::xAxis(int index) const { return x_[index]; }

// Return x axis vector
std::vector<double> &Data2D::xAxis()
{
    ++version_;

    return x_;
}

const std::vector<double> &Data2D::xAxis() const { return x_; }

// Return y value specified
double &Data2D::yAxis(int index)
{
    ++version_;

    return y_[index];
}

const double &Data2D::yAxis(int index) const { return y_[index]; }

// Return y Array
std::vector<double> &Data2D::yAxis()
{
    ++version_;

    return y_;
}

const std::vector<double> &Data2D::yAxis() const { return y_; }

// Return value specified
double &Data2D::value(int xIndex, int yIndex)
{
    ++version_;

    return values_[{xIndex, yIndex}];
}

const double &Data2D::value(int xIndex, int yIndex) const { return values_[{xIndex, yIndex}]; }

// Return two-dimensional values Array
Array2D<double> &Data2D::values()
{
    ++version_;

    return values_;
}

const Array2D<double> &Data2D::values() const { return values_; }

// Return value specified from linear array
double Data2D::value(int index) { return values_[index]; }

// Return number of values present in whole dataset
int Data2D::nValues() const { return values_.size(); }

// Return minimum value over all data points
double Data2D::minValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::min_element(values_.begin(), values_.end());
}

// Return maximum value over all data points
double Data2D::maxValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::max_element(values_.begin(), values_.end());
}

// Add / initialise errors array
void Data2D::addErrors()
{
    if (hasError_)
        Messenger::warn("Adding an error array to a Data2D that already has one...\n");

    errors_.initialise(x_.size(), y_.size());

    hasError_ = true;

    ++version_;
}

// Return whether the values have associated errors
bool Data2D::valuesHaveErrors() const { return hasError_; }

// Return error value specified
double &Data2D::error(int xIndex, int yIndex)
{
    assert(hasError_);

    ++version_;

    return errors_[{xIndex, yIndex}];
}

const double &Data2D::error(int xIndex, int yIndex) const
{
    assert(hasError_);

    return errors_[{xIndex, yIndex}];
}

// Return two-dimensional errors array
Array2D<double> &Data2D::errors()
{
    assert(hasError_);

    ++version_;

    return errors_;
}

const Array2D<double> &Data2D::errors() const
{
    assert(hasError_);

    return errors_;
}

/*
 * Operators
 */

void Data2D::operator=(const Data2D &source)
{
    tag_ = source.tag_;
    x_ = source.x_;
    y_ = source.y_;
    values_ = source.values_;
    hasError_ = source.hasError_;
    errors_ = source.errors_;

    ++version_;
}

void Data2D::operator+=(const double delta)
{
    values_ += delta;

    ++version_;
}

void Data2D::operator-=(const double delta)
{
    values_ -= delta;

    ++version_;
}

void Data2D::operator*=(const double factor)
{
    values_ *= factor;
    if (hasError_)
        errors_ *= factor;

    ++version_;
}

void Data2D::operator/=(const double factor)
{
    values_ /= factor;
    if (hasError_)
        errors_ /= factor;

    ++version_;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool Data2D::deserialise(LineParser &parser)
{
    clear();

    // Read object tag
    if (parser.readNextLine(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    tag_ = parser.line();

    // Read axis sizes and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto xSize = parser.argi(0);
    auto ySize = parser.argi(1);
    auto errors = parser.argb(2);
    initialise(xSize, ySize, errors);

    // Read x axis
    for (auto &x : x_)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        x = parser.argd(0);
    }

    // Read y axis
    for (auto &y : y_)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        y = parser.argd(0);
    }

    // Read errors / valuse
    if (hasError_)
    {
        for (auto index : ArrayIndex2D(values_))
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            values_[index] = parser.argd(0);
            errors_[index] = parser.argd(1);
        }
    }
    else
    {
        for (auto index : ArrayIndex2D(values_))
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            values_[index] = parser.argd(0);
        }
    }

    return true;
}

// Write data through specified LineParser
bool Data2D::serialise(LineParser &parser) const
{
    // Write object tag
    if (!parser.writeLineF("{}\n", tag_))
        return false;

    // Write axis sizes and errors flag
    if (!parser.writeLineF("{}  {}  {}\n", x_.size(), y_.size(), DissolveSys::btoa(hasError_)))
        return false;

    // Write x axis array
    for (auto x : x_)
        if (!parser.writeLineF("{:e}\n", x))
            return false;

    // Write y axis array
    for (auto y : y_)
        if (!parser.writeLineF("{:e}\n", y))
            return false;

    // Write values / errors
    if (hasError_)
    {
        for (auto index : ArrayIndex2D(values_))
        {
            if (!parser.writeLineF("{:e}  {:e}\n", values_[index], errors_[index]))
                return false;
        }
    }
    else
    {
        for (auto index : ArrayIndex2D(values_))
        {
            if (!parser.writeLineF("{:e}  {:e}\n", values_[index], errors_[index]))
                return false;
        }
    }

    return true;
}

// Express as a serialisable value
SerialisedValue Data2D::serialise() const
{
    SerialisedValue result = {{"tag", tag_}, {"x", x_}, {"y", y_}, {"values", values_.linearArray()}};
    if (hasError_)
        result["errors"] = errors_.linearArray();
    return result;
}

// Read values from a serialisable value
void Data2D::deserialise(const SerialisedValue &node)
{
    tag_ = toml::find<std::string>(node, "tag");
    x_ = toml::find<std::vector<double>>(node, "x");
    y_ = toml::find<std::vector<double>>(node, "y");
    values_.initialise(x_.size(), y_.size());
    values_.linearArray() = toml::find<std::vector<double>>(node, "values");

    Serialisable::optionalOn(node, "errors",
                             [this](const auto errors)
                             {
                                 hasError_ = true;
                                 errors_.initialise(x_.size(), y_.size());
                                 errors_.linearArray() = toml::get<std::vector<double>>(errors);
                             });
}
