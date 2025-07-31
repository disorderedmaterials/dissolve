// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeMix.h"
#include "classes/neutronWeights.h"
#include "math/data1D.h"
#include "templates/array2D.h"

// Forward Declarations
class Configuration;
class Interpolator;

// Set of Partials
class PartialSet
{
    public:
    PartialSet() = default;
    PartialSet(const std::map<const Species *, double> &realSpeciesPopulations);
    ~PartialSet();

    /*
     * Partials Data
     */
    private:
    // AtomTypeMix used to generate matrices
    AtomTypeMix atomTypeMix_;
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    // Pair matrix, containing full atom-atom partial
    Array2D<Data1D> partials_;
    // Unbound matrix, containing atom-atom partial of unbound pairs
    Array2D<Data1D> unboundPartials_;
    // Bound matrix, containing atom-atom partial of bound pairs
    Array2D<Data1D> boundPartials_;
    // Bound flag matrix, specifying if bound partials are empty
    Array2D<char> emptyBoundPartials_;
    // Total functions
    Data1D boundTotal_, unboundTotal_, total_;
    // Check for full or half matrix
    bool half_{true};
    // Effective density
    double rho_;
    // Species populations
    std::map<const Species *, double> realSpeciesPopulations_;

    public:
    // Initialise
    void initialise(const AtomTypeMix &atomTypMix, bool half = true);
    // Reset partial arrays
    void reset();
    // Return number of AtomTypes used to generate matrices
    int nAtomTypes() const;
    // Return atom types mis
    const AtomTypeMix &atomTypeMix() const;
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full atom-atom partial specified
    Data1D &partial(int i, int j);
    const Data1D &partial(int i, int j) const;
    // Return atom-atom partial for unbound pairs
    Data1D &unboundPartial(int i, int j);
    const Data1D &unboundPartial(int i, int j) const;
    // Return atom-atom partial for bound pairs
    Data1D &boundPartial(int i, int j);
    const Data1D &boundPartial(int i, int j) const;
    // Return empty bound partial flag
    char &emptyBoundPartial(int i, int j);
    const char &emptyBoundPartial(int i, int j) const;
    // Return whether specified bound partial is empty
    bool isBoundPartialEmpty(int i, int j) const;
    // Sum partials into totals
    void formTotals(bool applyConcentrationWeights);
    // Sum partials into totals for TR
    void formTRTotals(NeutronWeights weights);
    // Return total function
    Data1D &total();
    const Data1D &total() const;
    // Return total bound function
    Data1D &boundTotal();
    const Data1D &boundTotal() const;
    // Effective density
    void setEffectiveDensity(double rho);
    double effectiveDensity() const;
    // Return total unbound function
    Data1D &unboundTotal();
    const Data1D &unboundTotal() const;
    // Return real species populations
    const std::map<const Species *, double> &realSpeciesPopulations() const;
    // Save all partials and total
    bool save(std::string_view prefix, std::string_view tag, std::string_view suffix, std::string_view abscissaUnits) const;

    /*
     * Manipulation
     */
    private:
    // Adjust all partials, adding specified delta to each
    void adjust(double delta);

    public:
    // Add in partials from source PartialSet to our own, with specified weighting
    bool addPartials(PartialSet &source, double weighting);

    /*
     * Operators
     */
    public:
    void operator+=(const double delta);
    void operator+=(const PartialSet &source);
    void operator-=(const double delta);
    void operator*=(const double factor);
    PartialSet operator*(const double factor) const;

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
