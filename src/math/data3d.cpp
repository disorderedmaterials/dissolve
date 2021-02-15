// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/data3d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/histogram3d.h"

// Static Members (ObjectStore)
template <class Data3D> RefDataList<Data3D, int> ObjectStore<Data3D>::objects_;
template <class Data3D> int ObjectStore<Data3D>::objectCount_ = 0;
template <class Data3D> int ObjectStore<Data3D>::objectType_ = ObjectInfo::Data3DObject;
template <class Data3D> std::string_view ObjectStore<Data3D>::objectTypeName_ = "Data3D";

Data3D::Data3D() : PlottableData(PlottableData::TwoAxisPlottable), ListItem<Data3D>(), ObjectStore<Data3D>(this)
{
    hasError_ = false;

    clear();
}

Data3D::~Data3D() {}

Data3D::Data3D(const Data3D &source) : PlottableData(PlottableData::TwoAxisPlottable), ObjectStore<Data3D>(this)
{
    (*this) = source;
}

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

// Return x axis Array
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

    return values_[{xIndex, yIndex, zIndex}];
}

const double &Data3D::value(int xIndex, int yIndex, int zIndex) const { return values_[{xIndex, yIndex, zIndex}]; }

// Return three-dimensional values Array
Array3D<double> &Data3D::values3D()
{
    ++version_;

    return values_;
}

const Array3D<double> &Data3D::values3D() const { return values_; }

// Return number of values present in whole dataset
int Data3D::nValues() const { return values_.linearArraySize(); }

// Return minimum value over all data points
double Data3D::minValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::min_element(values_.cbegin(), values_.cend());
}

// Return maximum value over all data points
double Data3D::maxValue() const
{
    if (values_.empty())
        return 0.0;

    return *std::max_element(values_.cbegin(), values_.cend());
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
    if (!hasError_)
    {
        static double dummy;
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }

    ++version_;

    return errors_[{xIndex, yIndex, zIndex}];
}

const double &Data3D::error(int xIndex, int yIndex, int zIndex) const
{
    if (!hasError_)
    {
        static double dummy;
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but error(int,int) was requested.\n",
                        name(), objectTag());
        return dummy;
    }

    return errors_[{xIndex, yIndex, zIndex}];
}

// Return three-dimensional errors Array
Array3D<double> &Data3D::errors3D()
{
    if (!hasError_)
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

const Array3D<double> &Data3D::errors3D() const
{
    if (!hasError_)
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    return errors_;
}

/*
 * Operators
 */

void Data3D::operator=(const Data3D &source)
{
    name_ = source.name_;
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
 * GenericItemBase Implementations
 */

// Return class name
std::string_view Data3D::itemClassName() { return "Data3D"; }

// Read data through specified LineParser
bool Data3D::read(LineParser &parser, CoreData &coreData)
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
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
            {
                for (auto z = 0; z < z_.size(); ++z)
                {
                    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                        return false;
                    values_[{x, y, z}] = parser.argd(0);
                    errors_[{x, y, z}] = parser.argd(1);
                }
            }
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
bool Data3D::write(LineParser &parser)
{
    // Write object tag and name
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{}\n", name()))
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
        for (auto x = 0; x < x_.size(); ++x)
        {
            for (auto y = 0; y < y_.size(); ++y)
            {
                for (auto z = 0; z < z_.size(); ++z)
                    // TODO: Turn into a single loop when we have an
                    // iterator combinator
                    if (!parser.writeLineF("{:e}  {:e}\n", values_[{x, y, z}], errors_[{x, y, z}]))
                        return false;
            }
        }
    }
    else
    {
        for (auto &value : values_)
            if (!parser.writeLineF("{:e}\n", value))
                return false;
    }

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool Data3D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    if (!procPool.broadcast(x_, root))
        return false;
    if (!procPool.broadcast(y_, root))
        return false;
    if (!procPool.broadcast(z_, root))
        return false;
    if (!procPool.broadcast(values_.linearArray(), root))
        return false;
    if (!procPool.broadcast(hasError_, root))
        return false;
    if (!procPool.broadcast(errors_.linearArray(), root))
        return false;
#endif
    return true;
}

// Check item equality
bool Data3D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.equality(x_))
        return Messenger::error("Data3D x axis values not equivalent.\n");
    if (!procPool.equality(y_))
        return Messenger::error("Data3D y axis values not equivalent.\n");
    if (!procPool.equality(z_))
        return Messenger::error("Data3D z axis values not equivalent.\n");
    if (!procPool.equality(values_.linearArray()))
        return Messenger::error("Data3D values not equivalent.\n");
    if (!procPool.equality(hasError_))
        return Messenger::error("Data3D error flag not equivalent.\n");
    if (!procPool.equality(errors_.linearArray()))
        return Messenger::error("Data3D error values not equivalent.\n");
#endif
    return true;
}
