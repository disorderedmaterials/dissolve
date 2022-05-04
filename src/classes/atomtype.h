// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/interactionpotential.h"
#include "data/elements.h"

#include <map>
#include <toml11/toml.hpp>
#include <vector>

// Short-range functional forms
class ShortRangeFunctions
{
    public:
    enum class Form
    {
        None,                 /* No short-range dispersive forces */
        LennardJones,         /* Lennard-Jones 12-6 form with Lorentz-Berthelot combination rules */
        LennardJonesGeometric /* Lennard-Jones 12-6 form with Geometric combination rules */
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};

// AtomType Definition
class AtomType
{
    public:
    AtomType(Elements::Element Z = Elements::Unknown);
    AtomType(std::string name);
    ~AtomType() = default;

    /*
     * Character
     */
    private:
    // Name
    std::string name_{"XX"};
    // Associated chemical element
    Elements::Element Z_;

    public:
    // Set name of AtomType
    void setName(std::string_view name);
    // Return name of AtomType
    std::string_view name() const;
    // Set chemical element
    void setZ(Elements::Element Z);
    // Return chemical element
    Elements::Element Z() const;

    /*
     * Interaction Parameters
     */
    private:
    // Short-range interaction potential
    InteractionPotential<ShortRangeFunctions> interactionPotential_;
    // Atomic charge
    double charge_{0.0};
    // Index of this type in the master type index
    int index_{-1};

    public:
    // Return short-range interaction potential
    InteractionPotential<ShortRangeFunctions> &interactionPotential();
    const InteractionPotential<ShortRangeFunctions> &interactionPotential() const;
    // Set atomic charge
    void setCharge(double q);
    // Return atomic charge
    double charge() const;
    // Set index of this type in the master type index
    void setIndex(int id);
    // Return index of this type in the master type index
    int index() const;
    // Return whether our parameters are the same as those provided
    bool sameParametersAs(const AtomType *other, bool checkCharge = false);

    // This method generates a 'pairPotentials' TOML node from the object's members
    toml::basic_value<toml::discard_comments, std::map, std::vector> serialize();
    // This method populates the object's members with values read from a 'pairPotentials' TOML node
    void deserialize(toml::value node);
};
