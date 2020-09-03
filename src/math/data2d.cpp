/*
    *** 2-Dimensional Data With Statistics
    *** src/math/data2d.cpp
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

#include "math/data2d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/data1d.h"
#include "math/histogram2d.h"

// Static Members (ObjectStore)
template <class Data2D> RefDataList<Data2D, int> ObjectStore<Data2D>::objects_;
template <class Data2D> int ObjectStore<Data2D>::objectCount_ = 0;
template <class Data2D> int ObjectStore<Data2D>::objectType_ = ObjectInfo::Data2DObject;
template <class Data2D> std::string_view ObjectStore<Data2D>::objectTypeName_ = "Data2D";

Data2D::Data2D() : PlottableData(PlottableData::TwoAxisPlottable), ListItem<Data2D>(), ObjectStore<Data2D>(this)
{
    hasError_ = false;

    clear();
}

Data2D::~Data2D() {}

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
    x_.initialise(xSize);
    y_.initialise(ySize);
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
    values_.initialise(x_.nItems(), y_.nItems());
    hasError_ = source.hasError_;
    if (hasError_)
        errors_.initialise(x_.nItems(), y_.nItems());
    else
        errors_.clear();

    ++version_;
}

// Initialise from supplied axis ranges
void Data2D::initialise(double xMin, double xMax, double xBin, double yMin, double yMax, double yBin, bool withError)
{
    double xRange = xMax - xMin;
    double yRange = yMax - yMin;
    auto nXBins = xRange / xBin;
    auto nYBins = yRange / yBin;

    // We will clamp the maxima to the nearest bin boundary (not less than the supplied axis maxima)
    if ((xMin + nXBins * xBin) < xMax)
        ++nXBins;
    if ((yMin + nYBins * yBin) < yMax)
        ++nYBins;

    // Create x_ axis array
    x_.initialise(nXBins);
    double xCentre = xMin + xBin * 0.5;
    for (int n = 0; n < nXBins; ++n, xCentre += xBin)
        x_[n] = xCentre;

    // Create y_ axis array
    y_.initialise(nYBins);
    double yCentre = yMin + yBin * 0.5;
    for (int n = 0; n < nYBins; ++n, yCentre += yBin)
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
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data2D::xAxis().\n", index);
        return dummy;
    }
#endif

    ++version_;

    return x_[index];
}

// Return x value specified (const)
double Data2D::constXAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x_ array in Data2D::constXAxis().\n", index);
        return 0.0;
    }
#endif
    return x_.constAt(index);
}

// Return x axis Array
Array<double> &Data2D::xAxis()
{
    ++version_;

    return x_;
}

// Return x axis Array (const)
const Array<double> &Data2D::constXAxis() const { return x_; }

// Return y value specified
double &Data2D::yAxis(int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y_ array in Data2D::yAxis().\n", index);
        return dummy;
    }
#endif
    ++version_;

    return y_[index];
}

// Return y value specified (const)
double Data2D::constYAxis(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y_ array in Data2D::constYAxis().\n", index);
        return 0.0;
    }
#endif
    return y_.constAt(index);
}

// Return y Array
Array<double> &Data2D::yAxis()
{
    ++version_;

    return y_;
}

// Return y axis Array (const)
const Array<double> &Data2D::constYAxis() const { return y_; }

// Return value specified
double &Data2D::value(int xIndex, int yIndex)
{
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data2D::value().\n", xIndex);
        return dummy;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data2D::value().\n", yIndex);
        return dummy;
    }
#endif
    ++version_;

    return values_.at(xIndex, yIndex);
}

// Return value specified (const)
double Data2D::constValue(int xIndex, int yIndex) const
{
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data2D::constValue().\n", xIndex);
        return 0.0;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data2D::constValue().\n", yIndex);
        return 0.0;
    }
#endif
    return values_.constAt(xIndex, yIndex);
}

// Return values Array
Array2D<double> &Data2D::values()
{
    ++version_;

    return values_;
}

// Return values Array (const)
const Array2D<double> &Data2D::constValues2D() const { return values_; }
// Return values array in linear format
double *Data2D::values2DLinear() { return values_.linearArray(); }

// Return value specified from linear array
double Data2D::value(int index)
{
    double *array = values2DLinear();
    return array[index];
}

// Return number of values present in whole dataset
int Data2D::nValues() const { return values_.linearArraySize(); }

// Return minimum value over all data points
double Data2D::minValue() const
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
double Data2D::maxValue() const
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
void Data2D::addErrors()
{
    if (hasError_)
        Messenger::warn("Adding an error array to a Data2D that already has one...\n");

    errors_.initialise(x_.nItems(), y_.nItems());

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
        static double dummy;
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but error(int) was requested.\n", name(),
                        objectTag());
        return dummy;
    }
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data2D::error().\n", xIndex);
        return dummy;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        static double dummy;
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data2D::error().\n", yIndex);
        return dummy;
    }
#endif

    ++version_;

    return errors_.at(xIndex, yIndex);
}

// Return error value specified (const)
double Data2D::constError(int xIndex, int yIndex) const
{
    if (!hasError_)
    {
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but constError(int,int) was requested.\n",
                        name(), objectTag());
        return 0.0;
    }
#ifdef CHECKS
    if ((xIndex < 0) || (xIndex >= x_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for x axis in Data2D::constError().\n", xIndex);
        return 0.0;
    }
    if ((yIndex < 0) || (yIndex >= y_.nItems()))
    {
        Messenger::error("OUT_OF_RANGE - Index {} is out of range for y axis in Data2D::constError().\n", yIndex);
        return 0.0;
    }
#endif

    return errors_.constAt(xIndex, yIndex);
}

// Return error Array
Array2D<double> &Data2D::errors()
{
    if (!hasError_)
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but errors() was requested.\n", name(),
                        objectTag());

    ++version_;

    return errors_;
}

// Return error Array (const)
const Array2D<double> &Data2D::constErrors2D() const
{
    if (!hasError_)
        Messenger::warn("This Data2D (name='{}', tag='{}') has no errors to return, but constErrors2D() was requested.\n",
                        name(), objectTag());

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
    for (int n = 0; n < values_.linearArraySize(); ++n)
        values_.linearValue(n) += delta;

    ++version_;
}

void Data2D::operator-=(const double delta)
{
    for (int n = 0; n < values_.linearArraySize(); ++n)
        values_.linearValue(n) -= delta;

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
 * GenericItemBase Implementations
 */

// Return class name
std::string_view Data2D::itemClassName() { return "Data2D"; }

// Read data through specified LineParser
bool Data2D::read(LineParser &parser, CoreData &coreData)
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

    // Read errors / valuse
    if (hasError_)
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                values_.at(x, y) = parser.argd(0);
                errors_.at(x, y) = parser.argd(1);
            }
        }
    }
    else
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
            {
                if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
                    return false;
                values_.at(x, y) = parser.argd(0);
            }
        }
    }

    return true;
}

// Write data through specified LineParser
bool Data2D::write(LineParser &parser)
{
    // Write object tag and name
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{}\n", name()))
        return false;

    // Write axis sizes and errors flag
    if (!parser.writeLineF("{}  {}  {}\n", x_.nItems(), y_.nItems(), DissolveSys::btoa(hasError_)))
        return false;

    // Write x axis array
    for (int x = 0; x < x_.nItems(); ++x)
        if (!parser.writeLineF("{:e}\n", x_[x]))
            return false;

    // Write y axis array
    for (int y = 0; y < y_.nItems(); ++y)
        if (!parser.writeLineF("{:e}\n", y_[y]))
            return false;

    // Write values / errors
    if (hasError_)
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
                if (!parser.writeLineF("{:e}  {:e}\n", values_.constAt(x, y), errors_.constAt(x, y)))
                    return false;
        }
    }
    else
    {
        for (int x = 0; x < x_.nItems(); ++x)
        {
            for (int y = 0; y < y_.nItems(); ++y)
                if (!parser.writeLineF("{:e}\n", values_.constAt(x, y)))
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
