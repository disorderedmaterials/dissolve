// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"

// Add AtomType with specified Element
std::shared_ptr<AtomType> Dissolve::addAtomType(Elements::Element Z) { return coreData_.addAtomType(Z); }

// Return number of AtomTypes in list
int Dissolve::nAtomTypes() const { return coreData_.nAtomTypes(); }

// Return core AtomTypes list
std::vector<std::shared_ptr<AtomType>> &Dissolve::atomTypes() { return coreData_.atomTypes(); }

// Return nth AtomType in list
std::shared_ptr<AtomType> Dissolve::atomType(int n) { return coreData_.atomType(n); }

// Search for AtomType by name
std::shared_ptr<AtomType> Dissolve::findAtomType(std::string_view name) const { return coreData_.findAtomType(name); }

// Clear all AtomTypes
void Dissolve::clearAtomTypes() { coreData_.atomTypes().clear(); }
