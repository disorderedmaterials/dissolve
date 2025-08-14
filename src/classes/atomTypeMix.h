// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/atomTypeData.h"
#include "templates/keyedVector.h"
#include "templates/optionalRef.h"
#include <set>
#include <vector>

// Forward Declarations
class AtomType;
class Isotopologues;

// AtomTypeMix
class AtomTypeMix
{
    public:
    AtomTypeMix() = default;
    ~AtomTypeMix() = default;

    private:
    // Isotope data per atom type
    KeyedVector<const AtomType *, AtomTypeData> mix_;
    // Exchangeable atom types
    std::set<const AtomType *> exchangeables_;
    // Total population added to mix
    double totalPopulation_{0.0};

    private:
    // Add/increase population of specified Isotope for AtomType
    void add(const AtomType *atomType, Sears91::Isotope isotope, double population);
    // Finalise, calculating fractional populations etc., and accounting for exchangeable sites in boundCoherent values
    void finalise(const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes);

    public:
    // Create mix from Isotopologues
    void create(const std::vector<Isotopologues> &isotopologues,
                const std::vector<std::shared_ptr<AtomType>> &exchangeableTypes);
    // Return types/topes map
    const KeyedVector<const AtomType *, AtomTypeData> &mix() const;
    // Return whether specified atom type is exchangeable
    bool isExchangeable(const AtomType *atomType) const;
    // Return indices of AtomType pair
    std::optional<std::pair<int, int>> indexOf(const AtomType *at1, const AtomType *at2) const;
    // Print AtomType populations
    void print() const;
};
