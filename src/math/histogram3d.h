/*
    *** 3-Dimensional Histogram
    *** src/math/histogram3d.h
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

#pragma once

#include "genericitems/base.h"
#include "math/data3d.h"
#include "math/sampleddouble.h"
#include "templates/array3d.h"
#include "templates/objectstore.h"

// Forward Declarations
class ProcessPool;

// One-Dimensional Histogram
class Histogram3D : public ListItem<Histogram3D>, public ObjectStore<Histogram3D>, public GenericItemBase
{
    public:
    Histogram3D();
    ~Histogram3D();
    Histogram3D(const Histogram3D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for x data (hard left-edge of first bin)
    double xMinimum_;
    // Maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double xMaximum_;
    // Bin width for x axis
    double xBinWidth_;
    // Number of bins along x
    int nXBins_;
    // Minimum value for y data (hard left-edge of first bin)
    double yMinimum_;
    // Maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double yMaximum_;
    // Bin width for y axis
    double yBinWidth_;
    // Number of bins along y
    int nYBins_;
    // Minimum value for z data (hard left-edge of first bin)
    double zMinimum_;
    // Maximum value for z data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double zMaximum_;
    // Bin width for z axis
    double zBinWidth_;
    // Number of bins along z
    int nZBins_;
    // Histogram bins
    Array3D<long int> bins_;
    // Array of bin centres along x
    Array<double> xBinCentres_;
    // Array of bin centres along y
    Array<double> yBinCentres_;
    // Array of bin centres along z
    Array<double> zBinCentres_;
    // Accumulated averages
    Array3D<SampledDouble> averages_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data3D accumulatedData_;

    private:
    // Update accumulated data
    void updateAccumulatedData();

    public:
    // Initialise with specified bin range
    void initialise(double xMinimum, double xMaximum, double xBinWidth, double yMinimum, double yMaximum, double yBinWidth,
                    double zMinimum, double zMaximum, double zBinWidth);
    // Zero histogram bins
    void zeroBins();
    // Return minimum value for x data (hard left-edge of first bin)
    double xMinimum() const;
    // Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double xMaximum() const;
    // Return x bin width
    double xBinWidth() const;
    // Return number of x bins
    int nXBins() const;
    // Return minimum value for y data (hard left-edge of first bin)
    double yMinimum() const;
    // Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double yMaximum() const;
    // Return x bin width
    double yBinWidth() const;
    // Return number of y bins
    int nYBins() const;
    // Return minimum value for z data (hard left-edge of first bin)
    double zMinimum() const;
    // Return maximum value for z data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double zMaximum() const;
    // Return z bin width
    double zBinWidth() const;
    // Return number of z bins
    int nZBins() const;
    // Bin specified value, returning success
    bool bin(double x, double y, double z);
    // Bin specified value (as Vec3), returning success
    bool bin(Vec3<double> v);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return Array of x centre-bin values
    const Array<double> &xBinCentres() const;
    // Return Array of y centre-bin values
    const Array<double> &yBinCentres() const;
    // Return Array of z centre-bin values
    const Array<double> &zBinCentres() const;
    // Return histogram data
    Array3D<long int> &bins();
    // Add source histogram data into local array
    void add(Histogram3D &other, int factor = 1);
    // Return accumulated (averaged) data
    const Data3D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const Histogram3D &source);

    /*
     * GenericItemBase Implementations
     */
    public:
    // Return class name
    static std::string_view itemClassName();
    // Read data through specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Sum histogram data onto all processes
    bool allSum(ProcessPool &procPool);
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
