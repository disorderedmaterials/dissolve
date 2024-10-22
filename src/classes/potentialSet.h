// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeMix.h"
#include "classes/neutronWeights.h"
#include "math/data1D.h"
#include "math/histogram1D.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"
#include "templates/array2D.h"

// Forward Declarations
class Configuration;
class Interpolator;

// Set of Partials
class PotentialSet
{
    public:
    PotentialSet();
    ~PotentialSet();

    /*
     * Potentials Data
     */
    private:
    // Fingerprint for these partials (e.g. reflecting Configuration indices at which they were calculated)
    std::string fingerprint_;
    struct EPData
    {
        Data1D ep;
        double count{0};
        std::shared_ptr<AtomType> at1, at2;
    };
    // Pair matrix, containing full atom-atom partial
    std::map<std::string, EPData> potentials_;

    public:
    // Set up PartialSet, including initialising histograms for g(r) use
    bool setUp(const AtomTypeMix &atomTypeMix, double rdfRange, double binWidth);
    // Set up PartialSet without initialising histogram arrays
    bool setUpPotentials(const AtomTypeMix &atomTypMix);
    // Reset partial arrays
    void reset();
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full atom-atom partial specified
    Data1D &potential(int i);
    const Data1D &potential(int i) const;
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
