// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeMix.h"
#include "classes/partialSet.h"
#include "math/histogram1D.h"
#include "templates/array2D.h"

// Set of Histograms
class HistogramSet
{
    public:
    HistogramSet() = default;
    ~HistogramSet();

    /*
     * Data
     */
    private:
    // AtomTypeMix used to generate matrices
    AtomTypeMix atomTypeMix_;
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    // Histograms used for calculating full atom-atom partials in r
    Array2D<Histogram1D> fullHistograms_;
    // Histograms used for calculating bound atom-atom partials in r
    Array2D<Histogram1D> boundHistograms_;
    // Histograms used for deriving unbound atom-atom partials in r
    Array2D<Histogram1D> unboundHistograms_;
    // Check for full or half matrix
    bool half_{true};

    public:
    // Set up histograms
    void initialise(const AtomTypeMix &atomTypeMix, double rdfRange, double binWidth);
    // Clear all histogram data
    void clear();
    // Zero histogram bins
    void zeroBins();
    // Return atom types mis
    const AtomTypeMix &atomTypeMix() const;
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full histogram specified
    Histogram1D &fullHistogram(int i, int j);
    // Return bound histogram specified
    Histogram1D &boundHistogram(int i, int j);
    // Return unbound histogram specified
    Histogram1D &unboundHistogram(int i, int j);

    /*
     * Manipulation
     */
    public:
    // Form partials from stored Histogram data
    void formPartials(PartialSet &partials, double boxVolume);
    // Calculate RDF from supplied Histogram and normalisation data
    static void calculateRDF(Data1D &destination, const Histogram1D &histogram, double boxVolume, int nCentres,
                             int nSurrounding, double multiplier);
};
