// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeData.h"
#include "templates/keyedVector.h"
#include "templates/optionalRef.h"
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class CoreData;
class LineParser;

// AtomTypeMix
class AtomTypeMix
{
    public:
    AtomTypeMix() = default;
    ~AtomTypeMix() = default;

    /*
     * Types
     */
    private:
    // Vector of AtomTypeData
    KeyedVector<const AtomType *, AtomTypeData> types_;

    public:
    // Clear all data
    void clear();
    // Add/increase population of specified Isotope for AtomType
    void add(const AtomType *atomType, Sears91::Isotope isotope, double population);
    // Finalise, calculating fractional populations etc.
    void finalise();
    // Finalise, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
    void finalise(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes);
    // Return types/topes map
    const KeyedVector<const AtomType *, AtomTypeData> &mix() const;
    // Return indices of AtomType pair
    std::optional<std::pair<int, int>> indexOf(const AtomType *at1, const AtomType *at2) const;
    // Return total population of all types
    double totalPopulation() const;
    // Print AtomType populations
    void print() const;
};
