// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/histogram3D.h"
#include "base/messenger.h"
#include "base/serialiserLibrary.h"
#include "math/histogram1D.h"

Histogram3D::Histogram3D()
{
    accumulatedData_.addErrors();

    clear();
}

Histogram3D::Histogram3D(const Vector3 &xMinMaxRange, const Vector3 &yMinMaxRange, const Vector3 &zMinMaxRange)
{
    initialise(xMinMaxRange.x, xMinMaxRange.y, xMinMaxRange.z, yMinMaxRange.x, yMinMaxRange.y, yMinMaxRange.z, zMinMaxRange.x,
               zMinMaxRange.y, zMinMaxRange.z);
}

Histogram3D::Histogram3D(const Histogram3D &source) { (*this) = source; }

// Clear Data
void Histogram3D::clear()
{
    xMinimum_ = 0.0;
    xMaximum_ = 0.0;
    xBinWidth_ = 0.0;
    nXBins_ = 0;
    yMinimum_ = 0.0;
    yMaximum_ = 0.0;
    yBinWidth_ = 0.0;
    nYBins_ = 0;
    zMinimum_ = 0.0;
    zMaximum_ = 0.0;
    zBinWidth_ = 0.0;
    nZBins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
    bins_.clear();
    bins_.clear();
    xBinCentres_.clear();
    yBinCentres_.clear();
    zBinCentres_.clear();
    averages_.clear();
}

/*
 * Data
 */

// Update accumulated data
void Histogram3D::updateAccumulatedData()
{
    // Store bin centres and accumulated averages in the object
    for (auto x = 0; x < nXBins_; ++x)
    {
        accumulatedData_.xAxis(x) = xBinCentres_[x];
        for (auto y = 0; y < nYBins_; ++y)
        {
            if (x == 0)
                accumulatedData_.yAxis(y) = yBinCentres_[y];

            for (auto z = 0; z < nZBins_; ++z)
            {
                if (x == 0)
                    accumulatedData_.zAxis(z) = zBinCentres_[z];

                accumulatedData_.value(x, y, z) = averages_[std::tuple{x, y, z}];
                accumulatedData_.error(x, y, z) = averages_[std::tuple{x, y, z}].stDev();
            }
        }
    }
}

// Initialise with specified bin range
void Histogram3D::initialise(double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth,
                             double zMin, double zMax, double zBinWidth)
{
    clear();

    // Set up x axis
    xMinimum_ = xMin;
    xMaximum_ = xMax;
    xBinWidth_ = xBinWidth;
    Histogram1D::setUpAxis(xMinimum_, xMaximum_, xBinWidth_, nXBins_, xBinCentres_);

    // Set up y axis
    yMinimum_ = yMin;
    yMaximum_ = yMax;
    yBinWidth_ = yBinWidth;
    Histogram1D::setUpAxis(yMinimum_, yMaximum_, yBinWidth_, nYBins_, yBinCentres_);

    // Set up z axis
    zMinimum_ = zMin;
    zMaximum_ = zMax;
    zBinWidth_ = zBinWidth;
    Histogram1D::setUpAxis(zMinimum_, zMaximum_, zBinWidth_, nZBins_, zBinCentres_);

    // Create the main bins array
    bins_.initialise(nXBins_, nYBins_, nZBins_);
    averages_.initialise(nXBins_, nYBins_, nZBins_);

    // Set up the accumulated data array
    accumulatedData_.initialise(nXBins_, nYBins_, nZBins_, true);
    std::copy(xBinCentres_.begin(), xBinCentres_.end(), accumulatedData_.xAxis().begin());
    std::copy(yBinCentres_.begin(), yBinCentres_.end(), accumulatedData_.yAxis().begin());
    std::copy(zBinCentres_.begin(), zBinCentres_.end(), accumulatedData_.zAxis().begin());
}

// Zero histogram bins
void Histogram3D::zeroBins()
{
    bins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
}

// Return minimum value for x data (hard left-edge of first bin)
double Histogram3D::xMinimum() const { return xMinimum_; }

// Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::xMaximum() const { return xMaximum_; }

// Return x bin width
double Histogram3D::xBinWidth() const { return xBinWidth_; }

// Return number of x bins
int Histogram3D::nXBins() const { return nXBins_; }

// Return minimum value for y data (hard left-edge of first bin)
double Histogram3D::yMinimum() const { return yMinimum_; }

// Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::yMaximum() const { return yMaximum_; }

// Return y bin width
double Histogram3D::yBinWidth() const { return yBinWidth_; }

// Return number of y bins
int Histogram3D::nYBins() const { return nYBins_; }

// Bin specified value, returning success
bool Histogram3D::bin(double x, double y, double z)
{
    // Calculate target bin along x
    auto xBin = int((x - xMinimum_) / xBinWidth_);
    if ((xBin < 0) || (xBin >= nXBins_))
    {
        ++nMissed_;
        return false;
    }

    // Calculate target bin along y
    auto yBin = int((y - yMinimum_) / yBinWidth_);
    if ((yBin < 0) || (yBin >= nYBins_))
    {
        ++nMissed_;
        return false;
    }

    // Calculate target bin along z
    auto zBin = int((z - zMinimum_) / zBinWidth_);
    if ((zBin < 0) || (zBin >= nZBins_))
    {
        ++nMissed_;
        return false;
    }

    ++bins_[std::tuple{xBin, yBin, zBin}];
    ++nBinned_;

    return true;
}

// Bin specified value (as Vec3), returning success
bool Histogram3D::bin(Vector3 v) { return bin(v.x, v.y, v.z); }

// Return number of values binned over all bins
long int Histogram3D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void Histogram3D::accumulate()
{
    std::transform(averages_.begin(), averages_.end(), bins_.begin(), averages_.begin(),
                   [](auto avg, auto bin) { return avg + double(bin); });

    // Update accumulated data
    updateAccumulatedData();
}

// Return Array of x centre-bin values
const std::vector<double> &Histogram3D::xBinCentres() const { return xBinCentres_; }

// Return Array of y centre-bin values
const std::vector<double> &Histogram3D::yBinCentres() const { return yBinCentres_; }

// Return histogram data
Array3D<long int> &Histogram3D::bins() { return bins_; }

// Add source histogram data into local array
void Histogram3D::add(Histogram3D &other, int factor)
{
    if ((nXBins_ != other.nXBins_) || (nYBins_ != other.nYBins_))
    {
        Messenger::print("BAD_USAGE - Can't add Histogram3D data since arrays are not the same size ({}x{} vs {}x{}).\n",
                         nXBins_, nYBins_, other.nXBins_, other.nYBins_);
        return;
    }

    std::transform(bins_.begin(), bins_.end(), other.bins_.begin(), bins_.begin(),
                   [factor](auto bin, auto oth) { return bin + oth * factor; });
}

// Return accumulated (averaged) data
const Data3D &Histogram3D::accumulatedData() const { return accumulatedData_; }

/*
 * Operators
 */

void Histogram3D::operator=(const Histogram3D &source)
{
    xMinimum_ = source.xMinimum_;
    xMaximum_ = source.xMaximum_;
    xBinWidth_ = source.xBinWidth_;
    nXBins_ = source.nXBins_;
    yMinimum_ = source.yMinimum_;
    yMaximum_ = source.yMaximum_;
    yBinWidth_ = source.yBinWidth_;
    nYBins_ = source.nYBins_;
    nBinned_ = source.nBinned_;
    nMissed_ = source.nMissed_;
    bins_ = source.bins_;
    xBinCentres_ = source.xBinCentres_;
    yBinCentres_ = source.yBinCentres_;
    averages_ = source.averages_;
}

/*
 * Serialisation
 */

// Express as a serialisable value
void Histogram3D::serialise(std::string tag, SerialisedValue &target) const
{
    target[tag] = {{"xMinimum", xMinimum_},   {"xMaximum", xMaximum_},   {"xBinWidth", xBinWidth_}, {"yMinimum", yMinimum_},
                   {"yMaximum", yMaximum_},   {"yBinWidth", yBinWidth_}, {"zMinimum", zMinimum_},   {"zMaximum", zMaximum_},
                   {"zBinWidth", zBinWidth_}, {"nBinned", nBinned_},     {"nMissed", nMissed_}};
    Serialisable::vector(averages_.linearArray(), "averages", target[tag]);
}

// Read values from a serialisable value
void Histogram3D::deserialise(const SerialisedValue &node)
{
    clear();

    initialise(Deserialisable::deser<double>(node.at("xMinimum")), Deserialisable::deser<double>(node.at("xMaximum")),
               Deserialisable::deser<double>(node.at("yBinWidth")), Deserialisable ::deser<double>(node.at("yMinimum")),
               Deserialisable::deser<double>(node.at("yMaximum")), Deserialisable::deser<double>(node.at("yBinWidth")),
               Deserialisable::deser<double>(node.at("zMinimum")), Deserialisable::deser<double>(node.at("zMaximum")),
               Deserialisable::deser<double>(node.at("zBinWidth")));
    nBinned_ = Deserialisable::deser<long>(node.at("nBinned"));
    nMissed_ = Deserialisable::deser<long>(node.at("nMissed"));
    averages_.linearArray() = Deserialisable::vector<SampledDouble>(node.at("averages"));

    updateAccumulatedData();
}
