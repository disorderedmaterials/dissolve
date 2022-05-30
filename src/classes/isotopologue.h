// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
class Isotopologue : public Serialisable
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
    // AtomType references and their assigned Isotopes
    std::vector<std::tuple<std::shared_ptr<AtomType>, Sears91::Isotope>> isotopes_;

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
    const std::vector<std::tuple<std::shared_ptr<AtomType>, Sears91::Isotope>> &isotopes() const;

    // Express as a tree node
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node, CoreData &coreData);
};
