/*
    *** 3-Dimensional Data With Statistics
    *** src/math/data3d.cpp
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
    x_.initialise(xSize);
    y_.initialise(ySize);
    z_.initialise(zSize);
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
    values_.initialise(x_.nItems(), y_.nItems(), z_.nItems());
    hasError_ = source.hasError_;
    if (hasError_)
        errors_.initialise(x_.nItems(), y_.nItems(), z_.nItems());
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
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data3D::xAxis().\n", index);
        return dummy;
    }
#endif

    ++version_;

    return x_[index];
}

// Return x value specified (const)
double Data3D::constXAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data3D::constXAxis().\n", index);
        return 0.0;
    }
#endif
    return x_.constAt(index);
}

// Return x axis Array
Array<double> &Data3D::xAxis()
{
    return x_;

    ++version_;
}

// Return x axis Array (const)
const Array<double> &Data3D::constXAxis() const { return x_; }

// Return y value specified
double &Data3D::yAxis(int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data3D::yAxis().\n", index);
        return dummy;
    }
#endif
    ++version_;

    return y_[index];
}

// Return y value specified (const)
double Data3D::constYAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data3D::constYAxis().\n", index);
        return 0.0;
    }
#endif
    return y_.constAt(index);
}

// Return y Array
Array<double> &Data3D::yAxis()
{
    ++version_;

    return y_;
}

// Return y axis Array (const)
const Array<double> &Data3D::constYAxis() const { return y_; }

// Return z value specified
double &Data3D::zAxis(int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= z_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z_ array in Data3D::zAxis().\n", index);
        return dummy;
    }
#endif
    ++version_;

    return z_[index];
}

// Return z value specified (const)
double Data3D::constZAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= z_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z_ array in Data3D::constZAxis().\n", index);
        return 0.0;
    }
#endif
    return z_.constAt(index);
}

// Return z Array
Array<double> &Data3D::zAxis()
{
    ++version_;

    return z_;
}

// Return z axis Array (const)
const Array<double> &Data3D::constZAxis() const { return z_; }

// Return value specified
double &Data3D::value(int xIndex, int yIndex, int zIndex)
{
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data3D::value().\n", xIndex);
        return dummy;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data3D::value().\n", yIndex);
        return dummy;
    }
    if ((zIndex < 0) || (zIndex >= z_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z axis in Data3D::value().\n", zIndex);
        return dummy;
    }
#endif
    ++version_;

    return values_.at(xIndex, yIndex, zIndex);
}

// Return value specified (const)
double Data3D::constValue(int xIndex, int yIndex, int zIndex) const
{
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data3D::constValue().\n", xIndex);
        return 0.0;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data3D::constValue().\n", yIndex);
        return 0.0;
    }
    if ((zIndex < 0) || (zIndex >= z_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z axis in Data3D::constValue().\n", zIndex);
        return 0.0;
    }
#endif
    return values_.constAt(xIndex, yIndex, zIndex);
}

// Return values Array
Array3D<double> &Data3D::values()
{
    ++version_;

    return values_;
}

// Return values Array (const)
const Array3D<double> &Data3D::constValues3D() const { return values_; }

// Return number of values present in whole dataset
int Data3D::nValues() const { return values_.linearArraySize(); }

// Return minimum value over all data points
double Data3D::minValue() const
{
    if (values_.linearArraySize() == 0)
        return 0.0;

    double value = values_.constLinearValue(0);
    for (int n = 1; n < values_.linearArraySize(); ++n)
        if (values_.constLinearValue(n) < value)
            value = values_.constLinearValue(n);

    return value;
}

// Return maximum value over all data points
double Data3D::maxValue() const
{
    if (values_.linearArraySize() == 0)
        return 0.0;

    double value = values_.constLinearValue(0);
    for (int n = 1; n < values_.linearArraySize(); ++n)
        if (values_.constLinearValue(n) > value)
            value = values_.constLinearValue(n);

    return value;
}

// Add / initialise errors array
void Data3D::addErrors()
{
    if (hasError_)
        Messenger::warn("Adding an error array to a Data3D that already has one...\n");

    errors_.initialise(x_.nItems(), y_.nItems(), z_.nItems());

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
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data3D::error().\n", xIndex);
        return dummy;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data3D::error().\n", yIndex);
        return dummy;
    }
    if ((zIndex < 0) || (zIndex >= z_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z axis in Data3D::error().\n", zIndex);
        return dummy;
    }
#endif
    ++version_;

    return errors_.at(xIndex, yIndex, zIndex);
}

// Return error value specified (const)
double Data3D::constError(int xIndex, int yIndex, int zIndex) const
{
    if (!hasError_)
    {
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but constError(int,int) was requested.\n",
                        name(), objectTag());
        return 0.0;
    }
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data3D::constError().\n", xIndex);
        return 0.0;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data3D::constError().\n", yIndex);
        return 0.0;
    }
    if ((zIndex < 0) || (zIndex >= z_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for z axis in Data3D::constError().\n", zIndex);
        return 0.0;
    }
#endif

    return errors_.constAt(xIndex, yIndex, zIndex);
}

// Return error Array
Array3D<double> &Data3D::errors()
{
    if (!hasError_)
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

// Return error Array (const)
const Array3D<double> &Data3D::constErrors3D() const
{
    if (!hasError_)
        Messenger::warn("This Data3D (name='{}', tag='{}') has no errors to return, but constErrors() was requested.\n", name(),
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
    for (int n = 0; n < values_.linearArraySize(); ++n)
        values_.linearValue(n) += delta;

    ++version_;
}

void Data3D::operator-=(const double delta)
{
    for (int n = 0; n < values_.linearArraySize(); ++n)
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
    for (int x = 0; x < x_.nItems(); ++x)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        x_[x] = parser.argd(0);
    }

    // Read y axis
    for (int y = 0; y < y_.nItems(); ++y)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        y_[y] = parser.argd(0);
    }

    // Read z axis
    for (int z = 0; z < z_.nItems(); ++z)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        z_[z] = parser.argd(0);
    }

    // Read errors / valuse
    if (hasError_)
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                for (int z = 0; z < z_.nItems(); ++z)
                {
                    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                        return false;
                    values_.at(x, y, z) = parser.argd(0);
                    errors_.at(x, y, z) = parser.argd(1);
                }
            }
        }
    }
    else
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                for (int z = 0; z < z_.nItems(); ++z)
                {
                    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                        return false;
                    values_.at(x, y, z) = parser.argd(0);
                }
            }
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
    if (!parser.writeLineF("{}  {}  {}  {}\n", x_.nItems(), y_.nItems(), z_.nItems(), DissolveSys::btoa(hasError_)))
        return false;

    // Write x axis array
    for (int x = 0; x < x_.nItems(); ++x)
        if (!parser.writeLineF("{:e}\n", x_[x]))
            return false;

    // Write y axis array
    for (int y = 0; y < y_.nItems(); ++y)
        if (!parser.writeLineF("{:e}\n", y_[y]))
            return false;

    // Write z axis array
    for (int z = 0; z < z_.nItems(); ++z)
        if (!parser.writeLineF("{:e}\n", z_[z]))
            return false;

    // Write values / errors
    if (hasError_)
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                for (int z = 0; z < z_.nItems(); ++z)
                    if (!parser.writeLineF("{:e}  {:e}\n", values_.constAt(x, y, z), errors_.constAt(x, y, z)))
                        return false;
            }
        }
    }
    else
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                for (int z = 0; z < z_.nItems(); ++z)
                    if (!parser.writeLineF("{:e}\n", values_.constAt(x, y, z)))
                        return false;
            }
        }
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
    if (!procPool.broadcast(values_.linearArray(), values_.linearArraySize(), root))
        return false;
    if (!procPool.broadcast(hasError_, root))
        return false;
    if (!procPool.broadcast(errors_.linearArray(), errors_.linearArraySize(), root))
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
    if (!procPool.equality(values_.linearArray(), values_.linearArraySize()))
        return Messenger::error("Data3D values not equivalent.\n");
    if (!procPool.equality(hasError_))
        return Messenger::error("Data3D error flag not equivalent.\n");
    if (!procPool.equality(errors_.linearArray(), errors_.linearArraySize()))
        return Messenger::error("Data3D error values not equivalent.\n");
#endif
    return true;
}
