// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/configurationAtom.h"
#include "classes/atomType.h"
#include "classes/molecule.h"
#include "classes/speciesAtom.h"
#include <utility>

/*
 * Location
 */

// Set SpeciesAtom that this Atom represents
void ConfigurationAtom::setSpeciesAtom(const SpeciesAtom *spAtom) { speciesAtom_ = spAtom; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *ConfigurationAtom::speciesAtom() const { return speciesAtom_; }

// Set Molecule in which this Atom exists
void ConfigurationAtom::setMolecule(std::shared_ptr<Molecule> mol) { molecule_ = std::move(mol); }

// Return Molecule in which this Atom exists
const std::shared_ptr<Molecule> &ConfigurationAtom::molecule() const { return molecule_; }

// Set cell in which the atom exists
void ConfigurationAtom::setCell(Cell *cell) { cell_ = cell; }

// Return cell in which the atom exists
Cell *ConfigurationAtom::cell() const { return cell_; }

/*
 * Intramolecular Information
 */

// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition ConfigurationAtom::scaling(const ConfigurationAtom *j) const
{
    assert(speciesAtom_ != nullptr);
    assert(j != nullptr);
    assert(j->speciesAtom() != nullptr);

    return speciesAtom_->scaling(j->speciesAtom());
}
