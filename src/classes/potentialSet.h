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
    // Reset partial arrays
    void reset();
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of partials
    std::string_view fingerprint() const;
    // Return full atom-atom partial specified
    std::map<std::string, EPData> &potential();
    const std::map<std::string, EPData> &potential() const;

    /*
     * Operators
     */
    public:
    void operator+=(const double delta);
    void operator+=(const PotentialSet &source);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
