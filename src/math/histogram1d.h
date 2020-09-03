/*
    *** 1-Dimensional Histogram
    *** src/math/histogram1d.h
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
#include "math/data1d.h"
#include "math/sampleddouble.h"
#include "templates/array.h"
#include "templates/objectstore.h"

// Forward Declarations
class ProcessPool;

// One-Dimensional Histogram
class Histogram1D : public ListItem<Histogram1D>, public ObjectStore<Histogram1D>, public GenericItemBase
{
    public:
    Histogram1D();
    ~Histogram1D();
    Histogram1D(const Histogram1D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for data (hard left-edge of first bin)
    double minimum_;
    // Maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double maximum_;
    // Bin width
    double binWidth_;
    // Number of bins
    int nBins_;
    // Histogram bins
    Array<long int> bins_;
    // Array of bin centres
    Array<double> binCentres_;
    // Accumulated averages
    Array<SampledDouble> averages_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data1D accumulatedData_;

    private:
    // Update accumulated data
    void updateAccumulatedData();

    public:
    // Initialise with specified bin range
    void initialise(double minimum, double maximum, double binWidth);
    // Zero histogram bins
    void zeroBins();
    // Set up supplied axis
    static void setUpAxis(double axisMin, double &axisMax, double binWidth, int &nBins, Array<double> &binCentres);
    // Return minimum value for data (hard left-edge of first bin)
    double minimum() const;
    // Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double maximum() const;
    // Return bin width
    double binWidth() const;
    // Return number of bins
    int nBins() const;
    // Bin specified value, returning success
    bool bin(double x);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return Array of x centre-bin values
    const Array<double> &binCentres() const;
    // Return histogram data
    Array<long int> &bins();
    // Add source histogram data into local array
    void add(Histogram1D &other, int factor = 1);
    // Return accumulated (averaged) data
    const Data1D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const Histogram1D &source);

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
