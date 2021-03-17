// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/data2d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/data1d.h"
#include "math/histogram2d.h"
#include <algorithm>

// Static Members (ObjectStore)
template <class Data2D> RefDataList<Data2D, int> ObjectStore<Data2D>::objects_;
template <class Data2D> int ObjectStore<Data2D>::objectCount_ = 0;
template <class Data2D> int ObjectStore<Data2D>::objectType_ = ObjectInfo::Data2DObject;
template <class Data2D> std::string_view ObjectStore<Data2D>::objectTypeName_ = "Data2D";

Data2D::Data2D()
    : PlottableData(PlottableData::TwoAxisPlottable), ListItem<Data2D>(), ObjectStore<Data2D>(this), hasError_(false)
{
}

Data2D::Data2D(const Data2D &source) : PlottableData(PlottableData::TwoAxisPlottable), ObjectStore<Data2D>(this)
{
    (*this) = source;
}

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

// Return x axis Array
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
Array2D<double> &Data2D::values2D()
{
    ++version_;

    return values_;
}

const Array2D<double> &Data2D::values2D() const { return values_; }

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
    if (!hasError_)
    {
        static double dummy = 0.0;
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }

    ++version_;

    return errors_[{xIndex, yIndex}];
}

const double &Data2D::error(int xIndex, int yIndex) const
{
    if (!hasError_)
    {
        static double dummy = 0.0;
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but error(int,int) was requested.\n",
                        name(), objectTag());
        return dummy;
    }

    return errors_[{xIndex, yIndex}];
}

// Return two-dimensional errors Array
Array2D<double> &Data2D::errors2D()
{
    if (!hasError_)
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

const Array2D<double> &Data2D::errors2D() const
{
    if (!hasError_)
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but errors2D() was requested.\n", name(),
                        objectTag());

    return errors_;
}

/*
 * Operators
 */

void Data2D::operator=(const Data2D &source)
{
    name_ = source.name_;
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
    if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;
    setObjectTag(parser.line());

    // Read object name
    if (parser.readNextLine(LineParser::KeepBlanks) != LineParser::Success)
        return false;
    name_ = parser.line();

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
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                values_[{x, y}] = parser.argd(0);
                errors_[{x, y}] = parser.argd(1);
            }
        }
    }
    else
    {
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                values_[{x, y}] = parser.argd(0);
            }
        }
    }

    return true;
}

// Write data through specified LineParser
bool Data2D::serialise(LineParser &parser) const
{
    // Write object tag and name
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{}\n", name()))
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
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
                if (!parser.writeLineF("{:e}  {:e}\n", values_[{x, y}], errors_[{x, y}]))
                    return false;
        }
    }
    else
    {
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
                if (!parser.writeLineF("{:e}\n", values_[{x, y}]))
                    return false;
        }
    }

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool Data2D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(x_, root))
        return false;
    if (!procPool.broadcast(y_, root))
        return false;
    if (!procPool.broadcast(values_, root))
        return false;
    if (!procPool.broadcast(hasError_, root))
        return false;
    if (!procPool.broadcast(errors_, root))
        return false;
#endif
    return true;
}

// Check item equality
bool Data2D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(x_))
        return Messenger::error("Data2D x axis values not equivalent.\n");
    if (!procPool.equality(y_))
        return Messenger::error("Data2D y axis values not equivalent.\n");
    if (!procPool.equality(values_))
        return Messenger::error("Data2D values not equivalent.\n");
    if (!procPool.equality(hasError_))
        return Messenger::error("Data2D error flag not equivalent.\n");
    if (!procPool.equality(errors_))
        return Messenger::error("Data2D error values not equivalent.\n");
#endif
    return true;
}
