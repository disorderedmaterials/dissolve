// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <set>

// Forward Declarations
class AtomType;
class CoreData;

// Exchangeables - A set of AtomTypes that are exchangeable
class Exchangeables : public Serialisable<const CoreData &>
{
    public:
    Exchangeables() = default;
    Exchangeables(const std::vector<std::string_view> &atomTypeNames);
    ~Exchangeables() = default;

    /*
     * Data
     */
    private:
    // Exchangeable atom type names
    std::set<std::string> atomTypeNames_;

    public:
    // Clear all existing data
    void clear();
    // Add atom type
    void add(const std::string_view atomTypeName);
    // Remove the specified atom type
    void remove(const std::string_view atomTypeName);
    // Return whether the specified atom type name is present
    bool contains(const std::string_view atomTypeName) const;
    // Return vector of all exchangeables
    std::set<std::string> &exchangeables();
    const std::set<std::string> &exchangeables() const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
