// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#include "classes/partialset.h"
#include "math/histogram1d.h"

#include <vector>

namespace RDFModuleHelpers
{
struct TempHistogram
// Currently having to create a temp histogram struct as the normal histogram sometimes throws an exception in the data object
// store while in the parallel computations
{
    TempHistogram() = default;
    void initialise(Histogram1D &histo)
    {
        this->nBinned_ = histo.nBinned();
        this->nBins_ = histo.nBins();
        this->binCentres_ = histo.binCentres();
        this->bins_ = histo.bins();
        this->binWidth_ = histo.binWidth();
        this->minimum_ = histo.minimum();
        this->maximum_ = histo.maximum();
    }
    // Bin specified value, returning success
    bool bin(double x)
    {
        // Calculate target bin
        auto bin = int((x - minimum_) / binWidth_);

        // Check bin range
        if ((bin < 0) || (bin >= nBins_))
        {
            ++nMissed_;
            return false;
        }

        ++bins_[bin];
        ++nBinned_;

        return true;
    }
    TempHistogram operator+(const TempHistogram &other) const
    {
        TempHistogram ret = *this;
        for (auto n = 0; n < nBins_; ++n)
            ret.bins_[n] += other.bins_[n];

        ret.nBinned_ = this->nBinned_ + other.nBinned_;
        ret.nMissed_ = this->nMissed_ + other.nMissed_;
        ret.nBins_ = this->nBins_;

        return ret;
    }

    // Minimum value for data (hard left-edge of first bin)
    double minimum_;
    // Maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    double maximum_;
    // Bin width
    double binWidth_;
    // Number of bins
    int nBins_;
    // Histogram bins
    std::vector<long int> bins_;
    // Array of bin centres
    std::vector<double> binCentres_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
};

struct PartialHistograms
{
    PartialHistograms() = default;
    PartialHistograms(PartialSet &partialSet)
    {

        histograms_.initialise(partialSet.nAtomTypes(), partialSet.nAtomTypes());
        for (auto i = 0; i < partialSet.nAtomTypes(); ++i)
            for (auto j = 0; j < partialSet.nAtomTypes(); ++j)
                histograms_[{i, j}].initialise(partialSet.fullHistogram(i, j));
        // histograms_[{i, j}] = partialSet.fullHistogram(i, j);
    }
    PartialHistograms operator+(const PartialHistograms &other) const
    {
        PartialHistograms ret;
        ret.histograms_.initialise(this->histograms_.nRows(), this->histograms_.nColumns());
        for (auto i = 0; i < this->histograms_.nRows(); ++i)
            for (auto j = 0; j < this->histograms_.nColumns(); ++j)
            {
                ret.histograms_[{i, j}] = this->histograms_[{i, j}] + other.histograms_[{i, j}];
            }
        return ret;
    }

    void addToPartialSet(PartialSet &target)
    {
        for (auto k = 0; k < target.nAtomTypes(); ++k)
            for (auto j = 0; j < target.nAtomTypes(); ++j)
            {
                auto &histo = target.fullHistogram(k, j);
                auto nBinned = histo.nBinned();
                auto &bins = histo.bins();
                for (auto n = 0; n < histo.nBins(); ++n)
                    bins[n] += histograms_[{k, j}].bins_[n];

                nBinned += histograms_[{k, j}].nBinned_;
            }
    }

    Array2D<TempHistogram> histograms_;
};

} // namespace RDFModuleHelpers