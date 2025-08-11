// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/histogramSet.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "items/deserialisers.h"
#include "math/mathFunc.h"
#include "templates/algorithms.h"

/*
 * Data
 */

// Initialise histograms
void HistogramSet::initialise(const AtomTypeMix &atomTypeMix, double rdfRange, double binWidth)
{
    atomTypeMix_ = atomTypeMix;

    fullHistograms_.clear(half_);
    boundHistograms_.clear(half_);
    unboundHistograms_.clear(half_);

    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_,
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());

            fullHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
            boundHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
            unboundHistograms_.get(key).initialise(0.0, rdfRange, binWidth);
        },
        half_);
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

// Return atom types list
const AtomTypeMix &HistogramSet::atomTypeMix() const { return atomTypeMix_; }

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

/*
 * Manipulation
 */

// Create partials from stored Histogram data
void HistogramSet::formPartials(PartialSet &partials, double boxVolume)
{
    dissolve::for_each_pair(
        ParallelPolicies::seq, atomTypeMix_,
        [&](int n, const AtomTypeData &at1, int m, const AtomTypeData &at2)
        {
            DoubleKeyedMapKey key(at1.atomTypeName(), at2.atomTypeName());

            // Calculate RDFs from histogram data
            calculateRDF(partials.partials().get(key), fullHistograms_.get(key), boxVolume, at1.population(), at2.population(),
                         &at1 == &at2 ? 2.0 : 1.0);
            calculateRDF(partials.boundPartials().get(key), boundHistograms_.get(key), boxVolume, at1.population(),
                         at2.population(), &at1 == &at2 ? 2.0 : 1.0);
            calculateRDF(partials.unboundPartials().get(key), unboundHistograms_.get(key), boxVolume, at1.population(),
                         at2.population(), &at1 == &at2 ? 2.0 : 1.0);
        },
        half_);
}

// Calculate RDF from supplied Histogram and normalisation data
void HistogramSet::calculateRDF(Data1D &destination, const Histogram1D &histogram, double boxVolume, int nCentres,
                                int nSurrounding, double multiplier)
{
    auto nBins = histogram.nBins();
    double delta = histogram.binWidth();
    const auto &bins = histogram.bins();

    destination.clear();

    double shellVolume, factor, r = 0.5 * delta, lowerShellLimit = 0.0, numberDensity = nSurrounding / boxVolume;
    for (auto n = 0; n < nBins; ++n)
    {
        shellVolume = (4.0 / 3.0) * M_PI * (pow(lowerShellLimit + delta, 3.0) - pow(lowerShellLimit, 3.0));
        factor = nCentres * (shellVolume * numberDensity);

        destination.addPoint(r, bins[n] * (multiplier / factor));

        r += delta;
        lowerShellLimit += delta;
    }
}
