// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/atomType.h"
#include "data/ff/ff.h"
#include <utility>

ForcefieldAtomType::ForcefieldAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                                       std::string_view description, double q, ShortRangeFunctions::Form parametersForm,
                                       std::string_view parameterString, std::string_view equivalentName)
    : Z_(Z), index_(index), name_(name), equivalentName_(equivalentName), description_(description), charge_(q)
{
    InteractionPotential<ShortRangeFunctions> srPotential(parametersForm);
    if (!srPotential.parseParameters(parameterString))
        throw(std::runtime_error(
            fmt::format("Failed to parse parameters string '{}' when constructing atom type '{}'.\n", parameterString, name_)));
    parameters_ = srPotential.parameters();

    neta_.setDefinitionString(netaDefinition);
}
ForcefieldAtomType::ForcefieldAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                                       std::string_view description, double q, const std::vector<double> &parameters,
                                       std::string_view equivalentName)
    : Z_(Z), index_(index), name_(name), equivalentName_(equivalentName), description_(description), parameters_(parameters),
      charge_(q)
{

    neta_.setDefinitionString(netaDefinition);
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
    charge_ = sourceType.charge_;

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
    charge_ = source.charge_;
}

ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &&source) noexcept
{
    Z_ = source.Z_;
    index_ = source.index_;
    name_ = source.name_;
    neta_.setDefinitionString(source.neta_.definitionString());
    description_ = source.description_;
    equivalentName_ = source.equivalentName_;
    parameters_ = source.parameters_;
    charge_ = source.charge_;
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

// Return parameters vector
const std::vector<double> &ForcefieldAtomType::parameters() const { return parameters_; }

// Return parameter with index specified
double ForcefieldAtomType::parameter(int index) const { return parameters_[index]; }

// Return atomic charge
double ForcefieldAtomType::charge() const { return charge_; }
