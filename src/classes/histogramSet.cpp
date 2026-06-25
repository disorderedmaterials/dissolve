// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/histogramSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "math/mathFunc.h"
#include "templates/algorithms.h"

/*
 * Data
 */

// Initialise histograms
void HistogramSet::initialise(const std::vector<const AtomType *> &types, double rdfRange, double binWidth)
{
    atomTypes_ = types;

    fullHistograms_ = DoubleKeyedMap<Histogram1D>(triangular_);
    boundHistograms_ = DoubleKeyedMap<Histogram1D>(triangular_);
    unboundHistograms_ = DoubleKeyedMap<Histogram1D>(triangular_);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypes_,
        [&](int indexI, const auto *atomTypeI, int indexJ, const auto *atomTypeJ)
        {
            DoubleKeyedMapKey key(atomTypeI->name(), atomTypeJ->name());

            fullHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
            boundHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
            unboundHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
        },
        triangular_);
}

// Clear all histogram data
void HistogramSet::clear()
{
    fullHistograms_.clear();
    boundHistograms_.clear();
    unboundHistograms_.clear();
}

// Zero histogram bins
void HistogramSet::zeroBins()
{
    for (auto &histo : std::views::values(fullHistograms_.map()))
        histo.zeroBins();
    for (auto &histo : std::views::values(boundHistograms_.map()))
        histo.zeroBins();
    for (auto &histo : std::views::values(unboundHistograms_.map()))
        histo.zeroBins();
}

// Set new fingerprint
void HistogramSet::setFingerprint(std::string_view fingerprint) { fingerprint_ = fingerprint; }

// Return fingerprint of partials
std::string_view HistogramSet::fingerprint() const { return fingerprint_; }

// Return full histogram
DoubleKeyedMap<Histogram1D> &HistogramSet::fullHistograms() { return fullHistograms_; }

// Return bound histogram
DoubleKeyedMap<Histogram1D> &HistogramSet::boundHistograms() { return boundHistograms_; }

// Return unbound histogram
DoubleKeyedMap<Histogram1D> &HistogramSet::unboundHistograms() { return unboundHistograms_; }
