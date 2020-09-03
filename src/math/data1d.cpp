/*
    *** 1-Dimensional Data With Statistics
    *** src/math/data1d.cpp
    Copyright T. Youngs 2012-2020

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

#include "math/data1d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/histogram1d.h"

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
    x_.initialise(size);
    values_.initialise(size);
    hasError_ = withError;
    if (hasError_)
        errors_.initialise(size);
    else
        errors_.clear();

    ++version_;
}

// Initialise to be consistent in size and x axis with supplied object
void Data1D::initialise(const Data1D &source)
{
    x_ = source.x_;
    values_.initialise(x_.nItems());
    hasError_ = source.hasError_;
    if (hasError_)
        errors_.initialise(x_.nItems());
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
    values_ = 0.0;
    if (hasError_)
        errors_ = 0.0;

    ++version_;
}

// Return data version
int Data1D::version() const { return version_; }

// Add new data point
void Data1D::addPoint(double x, double value)
{
    x_.add(x);
    values_.add(value);

    if (hasError_)
        errors_.add(0.0);

    ++version_;
}

// Add new data point with error
void Data1D::addPoint(double x, double value, double error)
{
    x_.add(x);
    values_.add(value);

    if (hasError_)
        errors_.add(error);
    else
        Messenger::warn("Tried to addPoint() with an error to Data1D, but this Data1D (name='{}', tag='{}') has no "
                        "error information associated with it.\n",
                        name(), objectTag());

    ++version_;
}

// Remove first point
void Data1D::removeFirstPoint()
{
    if (values_.nItems() == 0)
        return;

    x_.removeFirst();
    values_.removeFirst();

    ++version_;
}

// Remove last point
void Data1D::removeLastPoint()
{
    if (values_.nItems() == 0)
        return;

    x_.removeLast();
    values_.removeLast();

    ++version_;
}

// Return x value specified
double &Data1D::xAxis(int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data1D::xAxis().\n", index);
        return dummy;
    }
#endif
    ++version_;

    return x_[index];
}

// Return x value specified (const)
double Data1D::constXAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data1D::constXAxis().\n", index);
        return 0.0;
    }
#endif
    return x_.constAt(index);
}

// Return x Array
Array<double> &Data1D::xAxis()
{
    ++version_;

    return x_;
}

// Return x axis Array (const)
const Array<double> &Data1D::constXAxis() const { return x_; }

// Return y value specified
double &Data1D::value(int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= values_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for values_ array in Data1D::value().\n", index);
        return dummy;
    }
#endif
    ++version_;

    return values_[index];
}

// Return y value specified (const)
double Data1D::constValue(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= values_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for values_ array in Data1D::constValue().\n", index);
        return 0.0;
    }
#endif
    return values_.constAt(index);
}

// Return y Array
Array<double> &Data1D::values()
{
    ++version_;

    return values_;
}

// Return y Array (const)
const Array<double> &Data1D::constValues() const { return values_; }

// Return number of values present in whole dataset
int Data1D::nValues() const { return x_.nItems(); }

// Return minimum value over all data points
double Data1D::minValue() const
{
    if (values_.nItems() == 0)
        return 0.0;

    double value = values_.constAt(0);
    for (int n = 1; n < values_.nItems(); ++n)
        if (values_.constAt(n) < value)
            value = values_.constAt(n);

    return value;
}

// Return maximum value over all data points
double Data1D::maxValue() const
{
    if (values_.nItems() == 0)
        return 0.0;

    double value = values_.constAt(0);
    for (int n = 1; n < values_.nItems(); ++n)
        if (values_.constAt(n) > value)
            value = values_.constAt(n);

    return value;
}

// Add / initialise errors array
void Data1D::addErrors()
{
    // 	if (hasError_) Messenger::warn("Adding an error array to a Data1D that already has one...\n");

    errors_.initialise(x_.nItems());

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
        static double dummy;
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }

    ++version_;

    return errors_.at(index);
}

// Return error value specified (const)
double Data1D::constError(int index) const
{
    if (!hasError_)
    {
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but constError(int) was requested.\n",
                        name(), objectTag());
        return 0.0;
    }

    return errors_.constAt(index);
}

// Return error Array
Array<double> &Data1D::errors()
{
    if (!hasError_)
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

// Return error Array (const)
const Array<double> &Data1D::constErrors() const
{
    if (!hasError_)
        Messenger::warn("This Data1D (name='{}', tag='{}') has no errors to return, but constErrors() was requested.\n", name(),
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
    if (x_.nItems() == 0)
    {
        copyArrays(source);
        return;
    }

    // Check array sizes
    if (x_.nItems() != source.x_.nItems())
    {
        Messenger::error("Can't += these Data1D together since they are of differing sizes.\n");
        return;
    }

    ++version_;

    // Loop over points, summing them into our array
    for (int n = 0; n < x_.nItems(); ++n)
    {
#ifdef CHECKS
        // Check x values for consistency
        if (fabs(x_[n] - source.constXAxis(n)) > 1.0e-6)
        {
            Messenger::error("Failed to += these Data1D together since the x arrays are different (at point {}, x "
                             "are {:e} and {:e}).\n",
                             n, x_[n], source.constXAxis(n));
            return;
        }
#endif
        values_[n] += source.constValue(n);
    }
}

void Data1D::operator+=(const double delta)
{
    for (int n = 0; n < values_.nItems(); ++n)
        values_[n] += delta;

    ++version_;
}

void Data1D::operator-=(const Data1D &source)
{
    // If no data is present, simply copy the other arrays and negate the y array
    if (x_.nItems() == 0)
    {
        copyArrays(source);
        for (int n = 0; n < values_.nItems(); ++n)
            values_[n] = -values_[n];
        return;
    }

    // Check array sizes
    if (x_.nItems() != source.x_.nItems())
    {
        Messenger::error("Can't -= these Data1D together since they are of differing sizes.\n");
        return;
    }

    ++version_;

    // Loop over points, summing them into our array
    for (int n = 0; n < x_.nItems(); ++n)
    {
#ifdef CHECKS
        // Check x values for consistency
        if (fabs(x_[n] - source.constXAxis(n)) > 1.0e-6)
        {
            Messenger::error("Failed to -= these Data1D together since the x arrays are different (at point {}, x "
                             "are {:e} and {:e}).\n",
                             n, x_[n], source.constXAxis(n));
            return;
        }
#endif
        values_[n] -= source.constValue(n);
    }
}

void Data1D::operator-=(const double delta)
{
    for (int n = 0; n < values_.nItems(); ++n)
        values_[n] -= delta;

    ++version_;
}

void Data1D::operator*=(const double factor)
{
    values_ *= factor;
    if (hasError_)
        errors_ *= factor;

    ++version_;
}

void Data1D::operator/=(const double factor)
{
    ++version_;

    values_ /= factor;
    if (hasError_)
        errors_ /= factor;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view Data1D::itemClassName() { return "Data1D"; }

// Read data through specified LineParser
bool Data1D::read(LineParser &parser, CoreData &coreData)
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
    for (int n = 0; n < nPoints; ++n)
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
bool Data1D::write(LineParser &parser)
{
    // Write object tag and name
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{}\n", name()))
        return false;

    // Write axis size and errors flag
    if (!parser.writeLineF("{} {}\n", x_.nItems(), DissolveSys::btoa(hasError_)))
        return false;

    // Write values / errors
    if (hasError_)
    {
        for (int n = 0; n < x_.nItems(); ++n)
            if (!parser.writeLineF("{}  {}  {}\n", x_[n], values_[n], errors_[n]))
                return false;
    }
    else
        for (int n = 0; n < x_.nItems(); ++n)
            if (!parser.writeLineF("{}  {}\n", x_[n], values_[n]))
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
