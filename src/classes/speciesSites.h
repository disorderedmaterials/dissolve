// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"

// Forward Declarations
class SpeciesSite;
class Species;

// SpeciesSites - Sites from one or more Species
class SpeciesSites : public Serialisable, ResolvableContext
{
    public:
    SpeciesSites() = default;
    SpeciesSites(const std::vector<const SpeciesSite *> &sites);
    ~SpeciesSites() = default;

    /*
     * Data
     */
    private:
    // Target sites
    ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const SpeciesSite *, double>> sites_;

    public:
    // Clear all existing data
    void clear();
    // Add site
    void add(const SpeciesSite *site);
    // Remove specified site from the list (if it exists)
    void remove(const SpeciesSite *site);
    // Return vector of all sites
    ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const SpeciesSite *, double>> &sites();
    const ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const SpeciesSite *, double>> &sites() const;
    // Return straight vector of all sites
    std::vector<const SpeciesSite *> getSpeciesSites() const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    // Resolve internal resolvable name references with supplied data
    void resolve(const std::map<std::string, const Species *> &speciesInScope) override;
};
