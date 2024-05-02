// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/data3D.h"
#include "math/sampledDouble.h"
#include "templates/array3D.h"

// Three-Dimensional Histogram
class Histogram3D
{
    public:
    Histogram3D();
    ~Histogram3D() = default;
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
    std::vector<double> xBinCentres_;
    // Array of bin centres along y
    std::vector<double> yBinCentres_;
    // Array of bin centres along z
    std::vector<double> zBinCentres_;
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
    const std::vector<double> &xBinCentres() const;
    // Return Array of y centre-bin values
    const std::vector<double> &yBinCentres() const;
    // Return Array of z centre-bin values
    const std::vector<double> &zBinCentres() const;
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
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;

    /*
     * Parallel Comms
     */
    public:
    // Sum histogram data onto all processes
    bool allSum(ProcessPool &procPool);
};
