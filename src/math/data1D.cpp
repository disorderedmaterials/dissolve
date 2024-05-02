// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/data1D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "templates/algorithms.h"
#include <cassert>

Data1D::Data1D() {}

Data1D::Data1D(const Data1D &source) { (*this) = source; }

Data1D::Data1D(const Data1DBase &source)
{
    x_ = source.xAxis();
    values_ = source.values();
    hasError_ = source.valuesHaveErrors();
    if (hasError_)
        errors_ = source.errors();
    else
        errors_.clear();
}

// Clear Data
void Data1D::clear()
{
    x_.clear();
    values_.clear();
    errors_.clear();
}

/*
 * Data
 */

// Set tag
void Data1D::setTag(std::string_view tag) { tag_ = tag; }

// Return tag
std::string_view Data1D::tag() const { return tag_; }

// Initialise arrays to specified size
void Data1D::initialise(int size, bool withError)
{
    x_.clear();
    x_.resize(size, 0.0);
    values_.clear();
    values_.resize(size, 0.0);
    hasError_ = withError;
    if (hasError_)
    {
        errors_.clear();
        errors_.resize(size, 0.0);
    }
    else
        errors_.clear();

    ++version_;
}

// Initialise to be consistent in size and x axis with supplied object
void Data1D::initialise(const Data1D &source)
{
    x_ = source.x_;
    values_.clear();
    values_.resize(x_.size(), 0.0);
    hasError_ = source.hasError_;
    if (hasError_)
    {
        errors_.clear();
        errors_.resize(x_.size(), 0.0);
    }
    else
        errors_.clear();

    ++version_;
}

// Copy arrays from supplied object
void Data1D::copyArrays(const Data1D &source)
{
    x_ = source.x_;
    values_ = source.values_;
    errors_ = source.errors_;
    hasError_ = source.hasError_;

    ++version_;
}

// Zero values array
void Data1D::zero()
{
    std::fill(values_.begin(), values_.end(), 0.0);
    if (hasError_)
        std::fill(errors_.begin(), errors_.end(), 0.0);

    ++version_;
}

// Return data version
int Data1D::version() const { return version_; }

// Add new data point
void Data1D::addPoint(double x, double value)
{
    x_.push_back(x);
    values_.push_back(value);

    if (hasError_)
        errors_.push_back(0.0);

    ++version_;
}

// Add new data point with error
void Data1D::addPoint(double x, double value, double error)
{
    assert(hasError_);

    x_.push_back(x);
    values_.push_back(value);
    errors_.push_back(error);

    ++version_;
}

// Remove first point
void Data1D::removeFirstPoint()
{
    if (values_.empty())
        return;

    x_.erase(x_.begin());
    values_.erase(values_.begin());
    if (hasError_)
        errors_.erase(values_.begin());

    ++version_;
}

// Remove last point
void Data1D::removeLastPoint()
{
    if (values_.empty())
        return;

    x_.pop_back();
    values_.pop_back();
    if (hasError_)
        errors_.pop_back();

    ++version_;
}

// Return x value specified
double &Data1D::xAxis(int index)
{
    ++version_;

    return x_[index];
}

const double &Data1D::xAxis(int index) const { return x_[index]; }

// Return x Array
std::vector<double> &Data1D::xAxis()
{
    ++version_;

    return x_;
}

const std::vector<double> &Data1D::xAxis() const { return x_; }

// Return y value specified
double &Data1D::value(int index)
{
    ++version_;

    return values_[index];
}

const double &Data1D::value(int index) const { return values_[index]; }

// Return values Array
std::vector<double> &Data1D::values()
{
    ++version_;

    return values_;
}

const std::vector<double> &Data1D::values() const { return values_; }

// Return number of values present in whole dataset
int Data1D::nValues() const { return x_.size(); }

// Return minimum value over all data points
double Data1D::minValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::min_element(values_.begin(), values_.end());
}

// Return maximum value over all data points
double Data1D::maxValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::max_element(values_.begin(), values_.end());
}

// Add / initialise errors array
void Data1D::addErrors()
{
    errors_.clear();
    errors_.resize(x_.size());

    hasError_ = true;

    ++version_;
}

// Return whether the values have associated errors
bool Data1D::valuesHaveErrors() const { return hasError_; }

// Return error value specified
double &Data1D::error(int index)
{
    assert(hasError_);

    ++version_;

    return errors_.at(index);
}

const double &Data1D::error(int index) const
{
    assert(hasError_);

    return errors_[index];
}

// Return error Array
std::vector<double> &Data1D::errors()
{
    assert(hasError_);

    ++version_;

    return errors_;
}

const std::vector<double> &Data1D::errors() const
{
    assert(hasError_);

    return errors_;
}

/*
 * Operators
 */

void Data1D::operator=(const Data1D &source)
{
    tag_ = source.tag_;
    x_ = source.x_;
    values_ = source.values_;
    hasError_ = source.hasError_;
    errors_ = source.errors_;

    ++version_;
}

void Data1D::operator+=(const Data1D &source)
{
    // If no data is present, simply copy the other arrays
    if (x_.empty())
    {
        copyArrays(source);
        ++version_;
        return;
    }

    // Check array sizes
    assert(x_.size() == source.x_.size());

    ++version_;

    // Loop over points, summing them into our array
    std::transform(source.values().begin(), source.values().end(), values_.begin(), values_.begin(), std::plus<>());
}

void Data1D::operator+=(const double delta)
{
    std::transform(values_.begin(), values_.end(), values_.begin(), [delta](auto value) { return value + delta; });

    ++version_;
}

void Data1D::operator-=(const Data1D &source)
{
    // If no data is present, simply copy the other arrays and negate the y array
    if (x_.empty())
    {
        copyArrays(source);
        std::transform(values_.begin(), values_.end(), values_.begin(), std::negate<>());
        ++version_;
        return;
    }

    assert(x_.size() == source.x_.size());

    // Loop over points, subtracting the source values from our array
    std::transform(values_.begin(), values_.end(), source.values().begin(), values_.begin(), std::minus<>());

    ++version_;
}

void Data1D::operator-=(const double delta)
{
    std::transform(values_.begin(), values_.end(), values_.begin(), [delta](auto value) { return value - delta; });

    ++version_;
}

void Data1D::operator*=(const double factor)
{
    std::transform(values_.begin(), values_.end(), values_.begin(), [factor](auto value) { return value * factor; });

    if (hasError_)
        std::transform(errors_.begin(), errors_.end(), errors_.begin(), [factor](auto error) { return error * factor; });

    ++version_;
}

void Data1D::operator*=(const std::vector<double> &factors)
{
    assert(x_.size() == factors.size());

    std::transform(values_.begin(), values_.end(), factors.begin(), values_.begin(), std::multiplies<>());
}

void Data1D::operator/=(const double factor)
{
    ++version_;

    std::transform(values_.begin(), values_.end(), values_.begin(), [factor](auto value) { return value / factor; });

    if (hasError_)
        std::transform(errors_.begin(), errors_.end(), errors_.begin(), [factor](auto error) { return error / factor; });
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool Data1D::deserialise(LineParser &parser)
{
    clear();

    // Read name
    if (parser.readNextLine(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    tag_ = parser.line();

    // Read number of points and whether errors are present
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nPoints = parser.argi(0);
    auto errors = parser.argb(1);
    initialise(nPoints, errors);

    // Read data points
    for (auto n = 0; n < nPoints; ++n)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        x_[n] = parser.argd(0);
        values_[n] = parser.argd(1);
        if (hasError_)
            errors_[n] = parser.argd(2);
    }

    return true;
}

// Write data through specified LineParser
bool Data1D::serialise(LineParser &parser) const
{
    // Write tag
    if (!parser.writeLineF("{}\n", tag_))
        return false;

    // Write axis size and errors flag
    if (!parser.writeLineF("{} {}\n", x_.size(), DissolveSys::btoa(hasError_)))
        return false;

    // Write values / errors
    if (hasError_)
    {
        for (auto &&[x, value, error] : zip(x_, values_, errors_))
            if (!parser.writeLineF("{}  {}  {}\n", x, value, error))
                return false;
    }
    else
        for (auto &&[x, value] : zip(x_, values_))
            if (!parser.writeLineF("{}  {}\n", x, value))
                return false;

    return true;
}

// Express as a serialisable value
SerialisedValue Data1D::serialise() const
{
    SerialisedValue result = {{"tag", tag_}, {"x", x_}, {"y", values_}};
    if (hasError_)
        result["errors"] = errors_;
    return result;
}

// Read values from a serialisable value
void Data1D::deserialise(const SerialisedValue &node)
{
    tag_ = toml::find<std::string>(node, "tag");
    x_ = toml::find<std::vector<double>>(node, "x");
    values_ = toml::find<std::vector<double>>(node, "y");

    Serialisable::optionalOn(node, "errors",
                             [this](const auto errors)
                             {
                                 hasError_ = true;
                                 errors_ = toml::get<std::vector<double>>(errors);
                             });
}
