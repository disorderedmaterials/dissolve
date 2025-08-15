// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/histogram1D.h"
#include "templates/array2D.h"
#include "templates/doubleKeyedMap.h"

// Set of Histograms
class HistogramSet
{
    public:
    HistogramSet() = default;
    ~HistogramSet() = default;

    /*
     * Data
     */
    private:
    // AtomTypes used to generate matrices
    std::vector<const AtomType *> atomTypes_;
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    // Histograms used for calculating full atom-atom partials in r
    DoubleKeyedMap<Histogram1D> fullHistograms_;
    // Histograms used for calculating bound atom-atom partials in r
    DoubleKeyedMap<Histogram1D> boundHistograms_;
    // Histograms used for deriving unbound atom-atom partials in r
    DoubleKeyedMap<Histogram1D> unboundHistograms_;
    // Check for full or half matrix
    bool half_{true};

    public:
    // Set up histograms
    void initialise(const std::vector<const AtomType *> &types, double rdfRange, double binWidth);
    // Clear all histogram data
    void clear();
    // Zero histogram bins
    void zeroBins();
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full histograms
    DoubleKeyedMap<Histogram1D> &fullHistograms();
    // Return bound histograms
    DoubleKeyedMap<Histogram1D> &boundHistograms();
    // Return unbound histograms
    DoubleKeyedMap<Histogram1D> &unboundHistograms();
};
