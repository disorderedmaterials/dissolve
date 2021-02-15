// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "data/elements.h"

AtomType::AtomType()
    : name_{"XX"}, Z_(Elements::Unknown), shortRangeType_(Forcefield::UndefinedType), exchangeable_(false), index_(-1)
{
}

/*
 * Character
 */

// Set name of AtomType
void AtomType::setName(std::string_view name) { name_ = name; }

// Return name of AtomType
std::string_view AtomType::name() const { return name_; }

// Set atomic element
void AtomType::setZ(Elements::Element Z) { Z_ = Z; }

// Return atomic element
Elements::Element AtomType::Z() const { return Z_; }

/*
 * Interaction Parameters
 */

// Set short-range interaction type
void AtomType::setShortRangeType(Forcefield::ShortRangeType srType) { shortRangeType_ = srType; }

// Return short-range interaction type
Forcefield::ShortRangeType AtomType::shortRangeType() const { return shortRangeType_; }

// Set short-range parameters vector
void AtomType::setShortRangeParameters(const std::vector<double> &parameters) { parameters_ = parameters; }

// Set single short-range parameter
void AtomType::setShortRangeParameter(int index, double parameter) { parameters_[index] = parameter; }

// Return short-range parameters vector
const std::vector<double> &AtomType::shortRangeParameters() const { return parameters_; }

// Return short-range parameter with index specified
double AtomType::shortRangeParameter(int index) const { return parameters_[index]; }

// Set atomic charge
void AtomType::setCharge(double q) { charge_ = q; }

// Return atomic charge
double AtomType::charge() const { return charge_; }

// Set index of this type in the main type index
void AtomType::setIndex(int id) { index_ = id; }

// Return index of this type in the main type index
int AtomType::index() const { return index_; }
