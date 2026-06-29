// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "templates/doubleKeyedMap.h"

// Forward Declarations
class AtomType;

// Set of Potentials
class PotentialSet : public Serialisable
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
    // Map of potentials
    DoubleKeyedMap<Data1D> potentials_;

    public:
    // Reset Potentials
    void reset();
    // Set new fingerprint
    void setFingerprint(std::string_view fingerprint);
    // Return fingerprint of potentials
    std::string_view fingerprint() const;
    // Return full set of potentials
    DoubleKeyedMap<Data1D> &potentials();
    const DoubleKeyedMap<Data1D> &potentials() const;

    /*
     * Operators
     */
    public:
    PotentialSet &operator+=(double delta);
    PotentialSet &operator+=(const PotentialSet &source);
    PotentialSet operator*(double factor) const;
    PotentialSet &operator*=(double factor);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(SerialisedValue node);
};
