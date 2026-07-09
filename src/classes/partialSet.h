// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/neutronWeights.h"
#include "math/data1D.h"
#include "templates/doubleKeyedMap.h"
#include "templates/optionalRef.h"
#include "templates/resolvable.h"

// Set of Partials
class PartialSet : ResolvableContext
{
    public:
    PartialSet() = default;
    ~PartialSet() = default;

    /*
     * Partials Data
     */
    private:
    // Species populations
    ResolvableKeyedVector<const Species *, double> realSpeciesPopulations_;
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    // Pair matrix, containing full atom-atom partial
    DoubleKeyedMap<Data1D> partials_;
    // Unbound matrix, containing atom-atom partial of unbound pairs
    DoubleKeyedMap<Data1D> unboundPartials_;
    // Bound matrix, containing atom-atom partial of bound pairs
    DoubleKeyedMap<Data1D> boundPartials_;
    // Total functions
    Data1D boundTotal_, unboundTotal_, total_;
    // Whether the partial set is a triangular matrix
    bool triangular_{true};
    // Effective density
    double rho_;

    public:
    // Initialise from supplied species populations
    void initialise(const KeyedVector<const Species *, int> &speciesPopulations, bool triangular = true);
    // Initialise from supplied real species populations
    void initialise(const KeyedVector<const Species *, double> &realSpeciesPopulations, bool triangular = true);
    // Initialise based on supplied PartialSet, templating all data
    void initialise(const PartialSet &partialSet);
    // Reset partial arrays
    void reset();
    // Return real species populations
    const ResolvableKeyedVector<const Species *, double> &realSpeciesPopulations() const;
    // Return fractional atom type populations
    KeyedVector<const AtomType *, double> atomTypeFractions() const;
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full atom-atom partials
    DoubleKeyedMap<Data1D> &partials();
    const DoubleKeyedMap<Data1D> &partials() const;
    // Return bound atom-atom partials
    DoubleKeyedMap<Data1D> &boundPartials();
    const DoubleKeyedMap<Data1D> &boundPartials() const;
    // Return unbound atom-atom partials
    DoubleKeyedMap<Data1D> &unboundPartials();
    const DoubleKeyedMap<Data1D> &unboundPartials() const;
    // Sum partials into totals
    void formTotals(bool applyConcentrationWeights);
    // Sum partials into totals for TR
    void formTRTotals(const NeutronWeights &weights);
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
    void addPartials(PartialSet &source, double weighting);

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
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(SerialisedValue node);
    // Resolve internal resolvable name references with supplied data
    void resolve(const std::map<std::string, const Species *> &speciesInScope) override;
};
