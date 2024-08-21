// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/data3D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "math/histogram3D.h"
#include "templates/array3D.h"
#include "templates/arrayIndex3D.h"

Data3D::Data3D() {}

Data3D::Data3D(const Data3D &source) { (*this) = source; }

// Clear Data
void Data3D::clear()
{
    x_.clear();
    y_.clear();
    z_.clear();
    values_.clear();
    errors_.clear();

    ++version_;
}

/*
 * Data
 */

// Set tag
void Data3D::setTag(std::string_view tag) { tag_ = tag; }

// Return tag
std::string_view Data3D::tag() const { return tag_; }

// Initialise arrays to specified size
void Data3D::initialise(int xSize, int ySize, int zSize, bool withError)
{
    x_.clear();
    x_.resize(xSize);
    y_.clear();
    y_.resize(ySize);
    z_.clear();
    z_.resize(zSize);
    values_.initialise(xSize, ySize, zSize);
    hasError_ = withError;
    if (hasError_)
        errors_.initialise(xSize, ySize, zSize);
    else
        errors_.clear();

    ++version_;
}

// Initialise arrays to specified uniform size
void Data3D::initialise(int size, bool withError) { initialise(size, size, size, withError); }

// Initialise bins and axes simultaneously
void Data3D::initialise(int nx, double xMin, double xDelta, int ny, double yMin, double yDelta, int nz, double zMin,
                        double zDelta, bool withError)
{
    // Set up bins
    initialise(nx, ny, nz, withError);

    // Set up axes
    x_.resize(nx);
    x_[0] = xMin + xDelta * 0.5;
    for (auto n = 1; n < nx; ++n)
        x_[n] = x_[n - 1] + xDelta;

    y_.resize(ny);
    y_[0] = yMin + yDelta * 0.5;
    for (auto n = 1; n < ny; ++n)
        y_[n] = y_[n - 1] + yDelta;

    z_.resize(nz);
    z_[0] = zMin + zDelta * 0.5;
    for (auto n = 1; n < nz; ++n)
        z_[n] = z_[n - 1] + zDelta;
}

// Initialise to be consistent in size and x axis with supplied object
void Data3D::initialise(const Data3D &source)
{
    x_ = source.x_;
    y_ = source.y_;
    z_ = source.z_;
    values_.initialise(x_.size(), y_.size(), z_.size());
    hasError_ = source.hasError_;
    if (hasError_)
        errors_.initialise(x_.size(), y_.size(), z_.size());
    else
        errors_.clear();

    ++version_;
}

// Copy arrays from supplied object
void Data3D::copyArrays(const Data3D &source)
{
    x_ = source.x_;
    y_ = source.y_;
    z_ = source.z_;
    values_ = source.values_;
    errors_ = source.errors_;
    hasError_ = source.hasError_;

    ++version_;
}

// Zero values array
void Data3D::zero()
{
    values_ = 0.0;
    if (hasError_)
        errors_ = 0.0;

    ++version_;
}

// Return data version
int Data3D::version() const { return version_; }

// Return x value specified
double &Data3D::xAxis(int index)
{
    ++version_;

    return x_[index];
}

const double &Data3D::xAxis(int index) const { return x_[index]; }

// Return x axis vector
std::vector<double> &Data3D::xAxis()
{
    ++version_;

    return x_;
}

const std::vector<double> &Data3D::xAxis() const { return x_; }

// Return y value specified
double &Data3D::yAxis(int index)
{
    ++version_;

    return y_[index];
}

const double &Data3D::yAxis(int index) const { return y_[index]; }

// Return y Array
std::vector<double> &Data3D::yAxis()
{
    ++version_;

    return y_;
}

const std::vector<double> &Data3D::yAxis() const { return y_; }

// Return z value specified
double &Data3D::zAxis(int index)
{
    ++version_;

    return z_[index];
}

const double &Data3D::zAxis(int index) const { return z_[index]; }

// Return z Array
std::vector<double> &Data3D::zAxis()
{
    ++version_;

    return z_;
}

const std::vector<double> &Data3D::zAxis() const { return z_; }

// Return value specified
double &Data3D::value(int xIndex, int yIndex, int zIndex)
{
    ++version_;

    return values_[std::tuple{xIndex, yIndex, zIndex}];
}

const double &Data3D::value(int xIndex, int yIndex, int zIndex) const { return values_[std::tuple{xIndex, yIndex, zIndex}]; }

// Return three-dimensional values Array
Array3D<double> &Data3D::values()
{
    ++version_;

    return values_;
}

const Array3D<double> &Data3D::values() const { return values_; }

// Return number of values present in whole dataset
int Data3D::nValues() const { return values_.linearArraySize(); }

// Return minimum value over all data points
double Data3D::minValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::min_element(values_.begin(), values_.end());
}

// Return maximum value over all data points
double Data3D::maxValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::max_element(values_.begin(), values_.end());
}

// Add / initialise errors array
void Data3D::addErrors()
{
    if (hasError_)
        Messenger::warn("Adding an error array to a Data3D that already has one...\n");

    errors_.initialise(x_.size(), y_.size(), z_.size());

    hasError_ = true;

    ++version_;
}

// Return whether the values have associated errors
bool Data3D::valuesHaveErrors() const { return hasError_; }

// Return error value specified
double &Data3D::error(int xIndex, int yIndex, int zIndex)
{
    assert(hasError_);

    ++version_;

    return errors_[std::tuple{xIndex, yIndex, zIndex}];
}

const double &Data3D::error(int xIndex, int yIndex, int zIndex) const
{
    assert(hasError_);

    return errors_[std::tuple{xIndex, yIndex, zIndex}];
}

// Return three-dimensional errors Array
Array3D<double> &Data3D::errors()
{
    assert(hasError_);

    ++version_;

    return errors_;
}

const Array3D<double> &Data3D::errors() const
{
    assert(hasError_);

    return errors_;
}

/*
 * Operators
 */

void Data3D::operator=(const Data3D &source)
{
    tag_ = source.tag_;
    x_ = source.x_;
    y_ = source.y_;
    z_ = source.z_;
    values_ = source.values_;
    hasError_ = source.hasError_;
    errors_ = source.errors_;

    ++version_;
}

void Data3D::operator+=(const double delta)
{
    for (auto n = 0; n < values_.linearArraySize(); ++n)
        values_.linearValue(n) += delta;

    ++version_;
}

void Data3D::operator-=(const double delta)
{
    for (auto n = 0; n < values_.linearArraySize(); ++n)
        values_.linearValue(n) -= delta;

    ++version_;
}

void Data3D::operator*=(const double factor)
{
    values_ *= factor;
    if (hasError_)
        errors_ *= factor;

    ++version_;
}

void Data3D::operator/=(const double factor)
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
bool Data3D::deserialise(LineParser &parser)
{
    clear();

    // Read object name
    if (parser.readNextLine(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    tag_ = parser.line();

    // Read axis sizes and initialise arrays
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto xSize = parser.argi(0);
    auto ySize = parser.argi(1);
    auto zSize = parser.argi(2);
    auto errors = parser.argb(3);
    initialise(xSize, ySize, zSize, errors);

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

    // Read z axis
    for (auto &z : z_)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        z = parser.argd(0);
    }

    // Read errors / valuse
    if (hasError_)
    {
        for (auto index : ArrayIndex3D(values_))
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            values_[index] = parser.argd(0);
            errors_[index] = parser.argd(1);
        }
    }
    else
    {
        for (auto &value : values_)
        {
            if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                return false;
            value = parser.argd(0);
        }
    }

    return true;
}

// Write data through specified LineParser
bool Data3D::serialise(LineParser &parser) const
{
    // Write tag
    if (!parser.writeLineF("{}\n", tag_))
        return false;

    // Write axis sizes and errors flag
    if (!parser.writeLineF("{}  {}  {}  {}\n", x_.size(), y_.size(), z_.size(), DissolveSys::btoa(hasError_)))
        return false;

    // Write x axis array
    for (auto x : x_)
        if (!parser.writeLineF("{:e}\n", x))
            return false;

    // Write y axis array
    for (auto y : y_)
        if (!parser.writeLineF("{:e}\n", y))
            return false;

    // Write z axis array
    for (auto z : z_)
        if (!parser.writeLineF("{:e}\n", z))
            return false;

    // Write values / errors
    if (hasError_)
    {
        for (auto index : ArrayIndex3D(values_))
        {
            if (!parser.writeLineF("{:e}  {:e}\n", values_[index], errors_[index]))
                return false;
        }
    }
    else
    {
        for (const auto &value : values_)
            if (!parser.writeLineF("{:e}\n", value))
                return false;
    }

    return true;
}
