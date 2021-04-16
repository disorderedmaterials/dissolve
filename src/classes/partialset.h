// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtypelist.h"
#include "classes/neutronweights.h"
#include "math/data1d.h"
#include "math/histogram1d.h"
#include "templates/array2d.h"
#include "templates/list.h"

// Forward Declarations
class Configuration;
class Interpolator;

// Set of Partials
class PartialSet
{
    public:
    PartialSet();
    ~PartialSet();

    /*
     * Partials Data
     */
    private:
    // AtomTypes used to generate matrices
    AtomTypeList atomTypes_;
    // RDF range used to initialise arrays
    double rdfRange_;
    // RDF bin width used to initialise arrays
    double rdfBinWidth_;
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    // Histograms used for calculating full atom-atom partials in r
    Array2D<Histogram1D> fullHistograms_;
    // Histograms used for calculating bound atom-atom partials in r
    Array2D<Histogram1D> boundHistograms_;
    // Histograms used for deriving unbound atom-atom partials in r
    Array2D<Histogram1D> unboundHistograms_;
    // Pair matrix, containing full atom-atom partial
    Array2D<Data1D> partials_;
    // Unbound matrix, containing atom-atom partial of unbound pairs
    Array2D<Data1D> unboundPartials_;
    // Bound matrix, containing atom-atom partial of bound pairs
    Array2D<Data1D> boundPartials_;
    // Bound flag matrix, specifying if bound partials are empty
    Array2D<char> emptyBoundPartials_;
    // Total function
    Data1D total_;

    public:
    // Set up PartialSet, including initialising histograms for g(r) use
    bool setUp(const AtomTypeList &atomTypes, double rdfRange, double binWidth);
    // Set up PartialSet without initialising histogram arrays
    bool setUpPartials(const AtomTypeList &atomTypes);
    // Set up histogram arrays for g(r) calculation
    void setUpHistograms(double rdfRange, double binWidth);
    // Reset partial arrays
    void reset();
    // Return number of AtomTypes used to generate matrices
    int nAtomTypes() const;
    // Return atom types list
    const AtomTypeList &atomTypes() const;
    // Return RDF range used to initialise arrays
    double rdfRange() const;
    // Return RDF bin width used to initialise arrays
    double rdfBinWidth() const;
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
    // Return full atom-atom partial specified
    Data1D &partial(int i, int j);
    const Data1D &partial(int i, int j) const;
    // Return atom-atom partial for unbound pairs
    Data1D &unboundPartial(int i, int j);
    const Data1D &unboundPartial(int i, int j) const;
    // Return atom-atom partial for bound pairs
    Data1D &boundPartial(int i, int j);
    const Data1D &boundPartial(int i, int j) const;
    // Return whether specified bound partial is empty
    bool isBoundPartialEmpty(int i, int j) const;
    // Sum partials into total
    void formTotal(bool applyConcentrationWeights);
    // Return total function
    Data1D &total();
    const Data1D &total() const;
    // Calculate and return total bound function
    Data1D boundTotal(bool applyConcentrationWeights) const;
    // Calculate and return total unbound function
    Data1D unboundTotal(bool applyConcentrationWeights) const;
    // Save all partials and total
    bool save(std::string_view prefix, std::string_view tag, std::string_view suffix, std::string_view abscissaUnits) const;
    // Name all object based on the supplied prefix
    void setObjectTags(std::string_view prefix, std::string_view suffix = "");
    // Return prefix applied to object names
    std::string_view objectNamePrefix() const;

    /*
     * Manipulation
     */
    private:
    // Adjust all partials, adding specified delta to each
    void adjust(double delta);

    public:
    // Form partials from stored Histogram data
    void formPartials(double boxVolume);
    // Add in partials from source PartialSet to our own, with specified weighting
    bool addPartials(PartialSet &source, double weighting);
    // Calculate RDF from supplied Histogram and normalisation data
    static void calculateRDF(Data1D &destination, Histogram1D &histogram, double boxVolume, int nCentres, int nSurrounding,
                             double multiplier);

    /*
     * Operators
     */
    public:
    void operator+=(const double delta);
    void operator+=(const PartialSet &source);
    void operator-=(const double delta);
    void operator*=(const double factor);

    /*
     * Searchers
     */
    public:
    // Return Data1D with specified tag, if it exists
    OptionalReferenceWrapper<const Data1D> searchData1D(std::string_view tag) const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
