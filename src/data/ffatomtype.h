// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/parameters.h"
#include "data/elements.h"
#include "neta/neta.h"
#include "templates/optionalref.h"

// Forward Declarations
class Forcefield;
class ForcefieldParameters;

// Forcefield AtomType Base Class
class ForcefieldAtomType
{
    public:
    ForcefieldAtomType(Elements::Element Z = Elements::Unknown, int index = -1, std::string_view name = "",
                       std::string_view netaDefinition = "", std::string_view description = "", double q = 0.0,
                       const std::vector<double> parameters = {});
    ForcefieldAtomType(OptionalReferenceWrapper<const ForcefieldParameters> params, Elements::Element Z = Elements::Unknown,
                       int index = -1, std::string_view name = "", std::string_view netaDefinition = "",
                       std::string_view description = "", double q = 0.0);
    ForcefieldAtomType(const ForcefieldAtomType &sourceType, std::string_view newTypeName, std::string_view netaDefinition = "",
                       std::string_view equivalentName = "");
    virtual ~ForcefieldAtomType() = default;
    ForcefieldAtomType(const ForcefieldAtomType &source);
    ForcefieldAtomType(const ForcefieldAtomType &&source);

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
    private:
    // Parameters that this atom type references (if any)
    OptionalReferenceWrapper<const ForcefieldParameters> parameterReference_;
    // Interatomic interaction parameters for this atom type
    InteractionParameters parameters_;

    public:
    // Return interatomic interaction parameters (referenced or otherwise)
    const InteractionParameters &parameters() const;
    // Return charge (from local parameters)
    double charge() const;
};
