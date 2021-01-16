// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/atomtype.h"
#include "data/ff/ff.h"
#include "data/ff/parameters.h"

ForcefieldAtomType::ForcefieldAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                                       std::string_view description, double q, std::vector<double> parameters)
    : Z_(Z), index_(index), name_(name), description_(description)
{
    neta_.setDefinitionString(netaDefinition);
    parameters_.setCharge(q);
    parameters_.setParameters(parameters);
}
ForcefieldAtomType::ForcefieldAtomType(OptionalReferenceWrapper<const ForcefieldParameters> params, Elements::Element Z,
                                       int index, std::string_view name, std::string_view netaDefinition,
                                       std::string_view description, double q)
    : Z_(Z), index_(index), name_(name), description_(description), parameterReference_(params)
{
    neta_.setDefinitionString(netaDefinition);
    parameters_.setCharge(q);
}
ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &sourceType, std::string_view newTypeName,
                                       std::string_view netaDefinition, std::string_view equivalentName)
{
    // Copy data from the supplied source
    Z_ = sourceType.Z_;
    index_ = sourceType.index_;
    name_ = newTypeName;
    neta_.setDefinitionString(netaDefinition);
    description_ = sourceType.description_;
    parameters_ = sourceType.parameters_;

    // Equivalent name provided?
    if (!equivalentName.empty())
        equivalentName_ = equivalentName;
}

ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &source)
{
    Z_ = source.Z_;
    index_ = source.index_;
    name_ = source.name_;
    neta_.setDefinitionString(source.neta_.definitionString());
    description_ = source.description_;
    equivalentName_ = source.equivalentName_;
    parameters_ = source.parameters_;
    parameterReference_ = source.parameterReference_;
}

ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &&source)
{
    Z_ = source.Z_;
    index_ = source.index_;
    name_ = source.name_;
    neta_.setDefinitionString(source.neta_.definitionString());
    description_ = source.description_;
    equivalentName_ = source.equivalentName_;
    parameters_ = source.parameters_;
    parameterReference_ = source.parameterReference_;
}

/*
 * Identity
 */

// Return element to which the atom type applies
Elements::Element ForcefieldAtomType::Z() const { return Z_; }

// Return index of type
int ForcefieldAtomType::index() const { return index_; }

// Return name of type
std::string_view ForcefieldAtomType::name() const { return name_; }

// Return equivalent name of type
std::string_view ForcefieldAtomType::equivalentName() const
{
    // Return defined equivalent name first, if defined
    if (!equivalentName_.empty())
        return equivalentName_;

    // If parameters are referenced, return their name. Otherwise, return ours
    if (parameterReference_)
    {
        const ForcefieldParameters &ffparams = *parameterReference_;
        return ffparams.name();
    }

    return name_;
}

// Return description for type
std::string_view ForcefieldAtomType::description() const { return description_; }

/*
 * Recognition
 */

// Create NETA definition for the atom type
bool ForcefieldAtomType::createNETA(const Forcefield *parentFF) { return neta_.create(parentFF); }

// Return NETA definition for the atom type
const NETADefinition &ForcefieldAtomType::neta() const { return neta_; }

/*
 * Parameters
 */

// Return interatomic interaction parameters
const InteractionParameters &ForcefieldAtomType::parameters() const
{
    // If reference parameters are defined, return those
    if (parameterReference_)
    {
        const ForcefieldParameters &ffparams = *parameterReference_;
        return ffparams.parameters();
    }

    return parameters_;
}

// Return charge (from local parameters)
double ForcefieldAtomType::charge() const { return parameters_.charge(); }
