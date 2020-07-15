/*
    *** ForcefieldAtomType Atom Type
    *** src/data/ffatomtype.cpp
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "data/ffatomtype.h"
#include "data/ff.h"
#include "data/ffparameters.h"
#include "neta/generator.h"

ForcefieldAtomType::ForcefieldAtomType(const Forcefield *parent, int Z, int index, const char *name, const char *netaDefinition,
                                       const char *description, double q, double data0, double data1, double data2,
                                       double data3)
    : ElementReference(Z)
{
    parent_ = parent;
    index_ = index;
    name_ = name;
    description_ = description;
    parameters_.setCharge(q);
    parameters_.setParameter(0, data0);
    parameters_.setParameter(1, data1);
    parameters_.setParameter(2, data2);
    parameters_.setParameter(3, data3);

    // Generate NETA
    if (netaDefinition && (!neta_.set(netaDefinition, parent)))
        Messenger::error("Failed to generate NETA for atom type '%s' in forcefield '%s' from string '%s'.\n", name_.get(),
                         parent ? parent->name() : "???", netaDefinition);
}
ForcefieldAtomType::ForcefieldAtomType(const Forcefield *parent, int Z, int index, const char *name, const char *netaDefinition,
                                       const char *description, double q, const char *parameterReference)
    : ElementReference(Z)
{
    parent_ = parent;
    index_ = index;
    name_ = name;
    description_ = description;
    parameters_.setCharge(q);
    parameterReference_ = parent->shortRangeParameters(parameterReference);
    if (!parameterReference_)
        Messenger::error("Reference parameters named '%s' are not defined in the forcefield '%s'.\n", parameterReference, parent->name());

    // Generate NETA
    if (netaDefinition && (!neta_.set(netaDefinition, parent)))
        Messenger::error("Failed to generate NETA for atom type '%s' in forcefield '%s' from string '%s'.\n", name_.get(),
                         parent ? parent->name() : "???", netaDefinition);
}
ForcefieldAtomType::ForcefieldAtomType(const Forcefield *parent, const ForcefieldAtomType &sourceType, const char *newTypeName,
                                       const char *netaDefinition, const char *equivalentName)
    : ElementReference(sourceType.Z())
{
    parent_ = parent;

    // Copy data from the supplied source
    index_ = sourceType.index_;
    name_ = newTypeName;
    description_ = sourceType.description_;
    parameters_ = sourceType.parameters_;

    // Generate NETA
    if (netaDefinition && (!neta_.set(netaDefinition, parent)))
        Messenger::error("Failed to generate NETA for copied atom type '%s' in forcefield '%s' from string '%s'.\n", name_.get(),
                         parent ? parent->name() : "???", netaDefinition);
    // Equivalent name provided?
    if (equivalentName)
        equivalentName_ = equivalentName;
}

ForcefieldAtomType::~ForcefieldAtomType() {}

ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &source) : ElementReference(source.Z())
{
    parent_ = source.parent_;
    index_ = source.index_;
    name_ = source.name_;
    description_ = source.description_;
    equivalentName_ = source.equivalentName_;
    parameters_ = source.parameters_;
    parameterReference_ = source.parameterReference_;
    neta_.set(source.neta_.definitionString(), parent_);
}

ForcefieldAtomType::ForcefieldAtomType(const ForcefieldAtomType &&source) : ElementReference(source.Z())
{
    parent_ = source.parent_;
    index_ = source.index_;
    name_ = source.name_;
    description_ = source.description_;
    equivalentName_ = source.equivalentName_;
    parameters_ = source.parameters_;
    parameterReference_ = source.parameterReference_;
    neta_.set(source.neta_.definitionString(), parent_);
}

/*
 * Identity
 */

// Return index of type
int ForcefieldAtomType::index() const { return index_; }

// Return name of type
const char *ForcefieldAtomType::name() const { return name_.get(); }

// Return equivalent name of type
const char *ForcefieldAtomType::equivalentName() const
{
    // Return defined equivalent name first, if defined
    if (!equivalentName_.isEmpty())
        return equivalentName_.get();

    // If parameters are referenced, return their name. Otherwise, return ours
    if (parameterReference_)
    {
        const ForcefieldParameters &ffparams = *parameterReference_;
        return ffparams.name();
    }

    return name_.get();
}

// Return description for type
const char *ForcefieldAtomType::description() const { return description_.get(); }

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
