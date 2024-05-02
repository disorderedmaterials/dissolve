// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
#include "data/elements.h"
#include "neta/neta.h"

// Forward Declarations
class Forcefield;

// Forcefield AtomType Base Class
class ForcefieldAtomType
{
    public:
    ForcefieldAtomType(Elements::Element Z = Elements::Unknown, int index = -1, std::string_view name = "",
                       std::string_view netaDefinition = "", std::string_view description = "", double q = 0.0,
                       ShortRangeFunctions::Form parametersForm = ShortRangeFunctions::Form::None,
                       std::string_view parameterString = "", std::string_view equivalentName = "");
    ForcefieldAtomType(Elements::Element Z = Elements::Unknown, int index = -1, std::string_view name = "",
                       std::string_view netaDefinition = "", std::string_view description = "", double q = 0.0,
                       const std::vector<double> &parameters = {}, std::string_view equivalentName = "");
    ForcefieldAtomType(const ForcefieldAtomType &sourceType, std::string_view newTypeName, std::string_view netaDefinition = "",
                       std::string_view equivalentName = "");
    virtual ~ForcefieldAtomType() = default;
    ForcefieldAtomType(const ForcefieldAtomType &source);
    ForcefieldAtomType(const ForcefieldAtomType &&source) noexcept;

    /*
     * Identity
     */
    private:
    // Element to which the atom type applies
    Elements::Element Z_;
    // Index of atom type
    int index_;
    // Name of atom type
    std::string name_;
    // Equivalent name, if defined
    std::string equivalentName_;
    // Brief description of type
    std::string description_;

    public:
    // Return element to which the atom type applies
    Elements::Element Z() const;
    // Return index of type
    int index() const;
    // Return name of type
    std::string_view name() const;
    // Return equivalent name of type
    std::string_view equivalentName() const;
    // Return description for type
    std::string_view description() const;

    /*
     * Recognition
     */
    private:
    // NETA definition for the atom type
    NETADefinition neta_;

    public:
    // Create NETA definition for the atom type
    bool createNETA(const Forcefield *parentFF);
    // Return NETA definition for the atom type
    const NETADefinition &neta() const;

    /*
     * Parameters
     */
    protected:
    // Vector of parameters
    std::vector<double> parameters_;
    // Atomic charge
    double charge_;

    public:
    // Return parameters vector
    const std::vector<double> &parameters() const;
    // Return parameter with index specified
    double parameter(int index) const;
    // Return atomic charge
    double charge() const;
};
