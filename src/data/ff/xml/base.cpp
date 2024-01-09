// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/xml/base.h"
#include <iostream>
#include <utility>

Forcefield_XML::Forcefield_XML(std::string name, std::vector<ForcefieldAtomType> &&atoms,
                               std::vector<ForcefieldBondTerm> &&bonds, std::vector<ForcefieldAngleTerm> &&angles,
                               std::vector<ForcefieldTorsionTerm> &&torsions, std::vector<ForcefieldImproperTerm> &&impropers)
    : Forcefield(atoms, bonds, angles, torsions, impropers), name_(std::move(name))
{
}

bool Forcefield_XML::setUp() { return true; }

std::string_view Forcefield_XML::name() const { return name_; }

std::string_view Forcefield_XML::description() const { return "Whatever forcefield you supplied"; }

std::string_view Forcefield_XML::publicationReferences() const { return "I have no idea"; }

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_XML::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }
