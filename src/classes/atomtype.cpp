// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "base/processpool.h"
#include "data/elements.h"
#include <string.h>

AtomType::AtomType()
{
    element_ = nullptr;
    name_ = "XX";
    exchangeable_ = false;
    shortRangeType_ = Forcefield::UndefinedType;
}

AtomType::~AtomType() {}

/*
 * Character
 */

// Set name of AtomType
void AtomType::setName(std::string_view name) { name_ = name; }

// Return name of AtomType
std::string_view AtomType::name() const { return name_; }

// Set atomic element
void AtomType::setElement(Elements::Element *el) { element_ = el; }

// Return atomic element
Elements::Element *AtomType::element() const { return element_; }

/*
 * Interaction Parameters
 */

// Set short-range interaction type
void AtomType::setShortRangeType(Forcefield::ShortRangeType srType) { shortRangeType_ = srType; }

// Return short-range interaction type
Forcefield::ShortRangeType AtomType::shortRangeType() const { return shortRangeType_; }

// Return interatomic interaction interaction parameters
InteractionParameters &AtomType::parameters() { return parameters_; }

// Set index of this type in the main type index
void AtomType::setIndex(int id) { index_ = id; }

// Return index of this type in the main type index
int AtomType::index() const { return index_; }
