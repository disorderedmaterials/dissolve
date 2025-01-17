// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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

// Set of Potentials
class PotentialSet
{
    public:
    PotentialSet();
    ~PotentialSet();

    /*
     * Potentials Data
     */
    private:
    // Fingerprint for these potentials
    std::string fingerprint_;
    struct PotentialData
    {
        Data1D potential;
        double count{0};
        std::shared_ptr<AtomType> at1, at2;
    };
    // Map of named potentials to data
    std::map<std::string, PotentialData> potentials_;

    public:
    // Reset Potentials
    void reset();
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of potentials
    std::string_view fingerprint() const;
    // Return full map of potentials specified
    std::map<std::string, PotentialData> &potentialMap();
    const std::map<std::string, PotentialData> &potentialMap() const;

    /*
     * Operators
     */
    public:
    PotentialSet &operator+=(const double delta);
    PotentialSet &operator+=(const PotentialSet &source);
    PotentialSet &operator*=(const double factor);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
