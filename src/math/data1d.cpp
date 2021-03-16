// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/data1d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/histogram1d.h"
#include "templates/algorithms.h"

// Static Members (ObjectStore)
template <class Data1D> RefDataList<Data1D, int> ObjectStore<Data1D>::objects_;
template <class Data1D> int ObjectStore<Data1D>::objectCount_ = 0;
template <class Data1D> int ObjectStore<Data1D>::objectType_ = ObjectInfo::Data1DObject;
template <class Data1D> std::string_view ObjectStore<Data1D>::objectTypeName_ = "Data1D";

Data1D::Data1D() : PlottableData(PlottableData::OneAxisPlottable), ListItem<Data1D>(), ObjectStore<Data1D>(this)
{
    static int count = 0;
    name_ = fmt::format("Data1D_{}", ++count);

    hasError_ = false;

    clear();
}

Data1D::~Data1D() {}

Data1D::Data1D(const Data1D &source) : PlottableData(PlottableData::OneAxisPlottable), ObjectStore<Data1D>(this)
{
    (*this) = source;
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

// Initialise arrays to specified size
void Data1D::initialise(int size, bool withError)
{
    x_.clear();
    x_.resize(size);
    values_.clear();
    values_.resize(size);
    hasError_ = withError;
    if (hasError_)
    {
        errors_.clear();
        errors_.resize(size);
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
    values_.resize(x_.size());
    hasError_ = source.hasError_;
    if (hasError_)
    {
        errors_.clear();
        errors_.resize(x_.size());
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
    x_.push_back(x);
    values_.push_back(value);

    if (hasError_)
        errors_.push_back(error);
    else
        Messenger::warn("Tried to addPoint() with an error to Data1D, but this Data1D (name='{}', tag='{}') has no "
                        "error information associated with it.\n",
                        name(), objectTag());

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
    if (!hasError_)
    {
        static double dummy = 0.0;
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }

    ++version_;

    return errors_.at(index);
}

const double &Data1D::error(int index) const
{
    if (!hasError_)
    {
        static double dummy;
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }

    return errors_[index];
}

// Return error Array
std::vector<double> &Data1D::errors()
{
    if (!hasError_)
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

const std::vector<double> &Data1D::errors() const
{
    if (!hasError_)
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    return errors_;
}

/*
 * Operators
 */

void Data1D::operator=(const Data1D &source)
{
    name_ = source.name_;
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
        return;
    }

    // Check array sizes
    if (x_.size() != source.x_.size())
    {
        Messenger::error("Can't += these Data1D together since they are of differing sizes.\n");
        return;
    }

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
        return;
    }

    // Check array sizes
    if (x_.size() != source.x_.size())
    {
        Messenger::error("Can't -= these Data1D together since they are of differing sizes.\n");
        return;
    }

    ++version_;

    // Loop over points, summing them into our array
    std::transform(values_.begin(), values_.end(), source.values().begin(), values_.begin(), std::minus<>());
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
    // Check array sizes
    if (x_.size() != factors.size())
    {
        Messenger::error("Can't *= this Array with Data1D values since they are of differing sizes.\n");
        return;
    }

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
bool Data1D::deserialise(LineParser &parser, const CoreData &coreData)
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
    // Write object tag and name
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{}\n", name()))
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

/*
 * Parallel Comms
 */

// Broadcast data
bool Data1D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(x_, root))
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
bool Data1D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(x_))
        return Messenger::error("Data1D x axis values not equivalent.\n");
    if (!procPool.equality(values_))
        return Messenger::error("Data1D y axis values not equivalent.\n");
    if (!procPool.equality(hasError_))
        return Messenger::error("Data1D error flag not equivalent.\n");
    if (!procPool.equality(errors_))
        return Messenger::error("Data1D error values not equivalent.\n");
#endif
    return true;
}
