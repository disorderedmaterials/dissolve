// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/sampledData1D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "math/histogram1D.h"
#include "templates/algorithms.h"
#include <cassert>

SampledData1D::SampledData1D() = default;

SampledData1D::SampledData1D(const SampledData1D &source) { (*this) = source; }

// Clear Data
void SampledData1D::clear()
{
    x_.clear();
    values_.clear();

    ++version_;
}

/*
 * Data
 */

// Set tag
void SampledData1D::setTag(std::string_view tag) { tag_ = tag; }

// Return tag
std::string_view SampledData1D::tag() const { return tag_; }

// Initialise arrays to specified size
void SampledData1D::initialise(int size)
{
    x_.clear();
    x_.resize(size);
    values_.initialise(size);

    ++version_;
}

// Initialise arrays, copying abscissa values
void SampledData1D::initialise(const std::vector<double> &xAxis)
{
    x_ = xAxis;
    values_.initialise(x_.size());

    ++version_;
}

// Zero values array
void SampledData1D::zero()
{
    values_.reset();

    ++version_;
}

// Return data version
int SampledData1D::version() const { return version_; }

// Return x value specified
const double &SampledData1D::xAxis(int index) const { return x_[index]; }

// Return x vector
const std::vector<double> &SampledData1D::xAxis() const { return x_; }

// Return value specified
const double &SampledData1D::value(int index) const { return values_.values()[index]; }

// Return value vector
const std::vector<double> &SampledData1D::values() const { return values_.values(); }

// Return number of values present in whole dataset
int SampledData1D::nValues() const { return x_.size(); }

// Return minimum value over all data points
double SampledData1D::minValue() const
{
    if (values_.values().empty())
        return 0.0;

    return *std::min_element(values_.values().begin(), values_.values().end());
}

// Return maximum value over all data points
double SampledData1D::maxValue() const
{
    if (values_.values().empty())
        return 0.0;

    return *std::max_element(values_.values().begin(), values_.values().end());
}

// Return whether the values have associated errors
bool SampledData1D::valuesHaveErrors() const { return true; }

// Return error value specified
const double &SampledData1D::error(int index) const { return values_.stDev()[index]; }

// Return error Array
const std::vector<double> &SampledData1D::errors() const { return values_.stDev(); }

/*
 * Operators
 */

SampledData1D &SampledData1D::operator=(const Data1D &source)
{
    clear();

    x_ = source.xAxis();
    values_ += source.values();

    ++version_;

    return *this;
}

void SampledData1D::operator+=(const Data1D &source)
{
    // If no data is present, copy the data
    if (x_.empty())
    {
        (*this) = source;
        return;
    }

    assert(x_.size() == source.xAxis().size());

    values_ += source.values();

    ++version_;
}

void SampledData1D::operator+=(const std::vector<double> &source)
{
    assert(x_.size() == source.size());

    values_ += source;

    ++version_;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool SampledData1D::deserialise(LineParser &parser)
{
    clear();

    // Read tag
    if (parser.readNextLine(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    tag_ = parser.line();

    // Read number of points and sample size
    if (parser.getArgsDelim(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    auto nPoints = parser.argi(0);
    auto count = parser.argi(1);

    // Read samples into temporary arrays, then set values_ at the end
    x_.reserve(nPoints);
    std::vector<double> mean, stDev, m2;
    mean.reserve(nPoints);
    stDev.reserve(nPoints);
    m2.reserve(nPoints);
    for (auto n = 0; n < nPoints; ++n)
    {
        if (parser.getArgsDelim(LineParser::KeepBlanks) != LineParser::Success)
            return false;

        x_.push_back(parser.argd(0));
        mean.push_back(parser.argd(1));
        stDev.push_back(parser.argd(2));
        m2.push_back(parser.argd(3));
    }
    values_.initialise(count, mean, stDev, m2);

    return true;
}

// Read value data only through specified LineParser
bool SampledData1D::deserialiseValues(LineParser &parser) { return values_.deserialise(parser); }

// Write data through specified LineParser
bool SampledData1D::serialise(LineParser &parser) const
{
    // Write tag
    if (!parser.writeLine(tag_))
        return false;

    // Write number of points and sample size
    if (!parser.writeLineF("{} {}  # nData count\n", x_.size(), values_.count()))
        return false;

    // Write four-column format : x, mean, stDev, m2
    for (auto &&[x, mean, m2, stDev] : zip(x_, values_.values(), values_.m2(), values_.stDev()))
        if (!parser.writeLineF("{} {} {} {}\n", x, mean, m2, stDev))
            return false;

    return true;
}

// Write value data only through specified LineParser
bool SampledData1D::serialiseValues(LineParser &parser) const { return values_.serialise(parser); }
