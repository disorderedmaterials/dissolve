// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "data/elements.h"
#include "data/isotopes.h"
#include <map>
#include <memory>
#include <tuple>
#include <vector>

// Forward Declarations
class AtomType;
class Species;
class CoreData;

/*
 * Isotopologue Definition
 */
class Isotopologue : public Serialisable<const CoreData &>
{
    public:
    Isotopologue(std::string name = "");
    ~Isotopologue() = default;

    /*
     * Basic Information
     */
    private:
    // Parent Species
    const Species *parent_{nullptr};
    // Descriptive name
    std::string name_;

    public:
    // Set parent Species
    void setParent(const Species *parent);
    // Return parent Species
    const Species *parent() const;
    // Set name of Isotopologue
    void setName(std::string_view name);
    // Return name of Isotopologue
    std::string_view name() const;

    /*
     * Isotope Definition
     */
    private:
    // AtomType names and their assigned Isotopes
    std::map<std::string, Sears91::IsotopeData> isotopes_;

    public:
    // Update current AtomType/Isotopes against parent Species
    void update();
    // Validate current AtomType/Isotopes against available AtomTypes
    void checkAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes);
    // Set AtomType/Isotope pair in list
    void setAtomTypeIsotope(std::shared_ptr<AtomType> at, Sears91::Isotope tope);
    // Return Isotope for specified AtomType
    Sears91::Isotope atomTypeIsotope(std::shared_ptr<AtomType> at) const;
    // Return AtomType/Isotope pairs list
    std::map<std::string, Sears91::IsotopeData> &isotopes();
    const std::map<std::string, Sears91::IsotopeData> &isotopes() const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
