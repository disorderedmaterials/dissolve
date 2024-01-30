// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/atomType.h"
#include "classes/molecule.h"
#include "classes/speciesAtom.h"
#include <utility>

// Set local AtomType index
void Atom::setLocalTypeIndex(int id) { localTypeIndex_ = id; }

// Return local AtomType index
int Atom::localTypeIndex() const { return localTypeIndex_; }

// Set master AtomType index
void Atom::setMasterTypeIndex(int id) { masterTypeIndex_ = id; }

// Return master AtomType index
int Atom::masterTypeIndex() const { return masterTypeIndex_; }

/*
 * Location
 */

// Set SpeciesAtom that this Atom represents
void Atom::setSpeciesAtom(const SpeciesAtom *spAtom) { speciesAtom_ = spAtom; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *Atom::speciesAtom() const { return speciesAtom_; }

// Set Molecule in which this Atom exists
void Atom::setMolecule(std::shared_ptr<Molecule> mol) { molecule_ = std::move(mol); }

// Return Molecule in which this Atom exists
const std::shared_ptr<Molecule> &Atom::molecule() const { return molecule_; }

/*
 * Intramolecular Information
 */

// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition Atom::scaling(const AtomRef j) const
{
    assert(speciesAtom_ != nullptr);
    assert(j.speciesAtom() != nullptr);

    return speciesAtom_->scaling(j.speciesAtom());
}

/*
 * Targeted Potentials
 */

// Add targeted potential to this atom
void Atom::addTargetedPotential(const ExternalPotential *potential) { targetedPotentials_.emplace_back(potential); }

// Clear all targeted potentials from this Atom
void Atom::clearTargetedPotentials() { targetedPotentials_.clear(); }

// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &Atom::targetedPotentials() const { return targetedPotentials_; }
