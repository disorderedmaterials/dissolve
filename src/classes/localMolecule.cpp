// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/localMolecule.h"
#include "classes/empiricalFormula.h"
#include "classes/species.h"

LocalMolecule::LocalMolecule(const LocalMolecule &copyFrom) { copyData(copyFrom); }

LocalMolecule::LocalMolecule(LocalMolecule &&moveFrom) { copyData(moveFrom); }

LocalMolecule &LocalMolecule::operator=(const LocalMolecule &copyFrom)
{
    copyData(copyFrom);
    return *this;
}

LocalMolecule &LocalMolecule::operator=(LocalMolecule &&moveFrom)
{
    copyData(moveFrom);
    return *this;
}

// Copy data from specified object
void LocalMolecule::copyData(const LocalMolecule &object)
{
    species_ = object.species_;

    localAtoms_ = object.localAtoms_;
    atoms_.resize(localAtoms_.size());
    std::transform(localAtoms_.begin(), localAtoms_.end(), atoms_.begin(), [](auto &atom) { return &atom; });
}

// Set Species that this Molecule represents
void LocalMolecule::setSpecies(const Species *sp)
{
    species_ = sp;

    localAtoms_.resize(sp->nAtoms());
    atoms_.resize(sp->nAtoms());
    std::transform(localAtoms_.begin(), localAtoms_.end(), atoms_.begin(), [](auto &atom) { return &atom; });

    for (auto &&[atom, spAtom] : zip(localAtoms_, species_->atoms()))
        atom.setSpeciesAtom(&spAtom);
}

// Add Atom to Molecule
void LocalMolecule::addAtom(Atom *atom) { throw(std::runtime_error("Can't addAtom() in a LocalMolecule.\n")); }

// Update local atom pointers from main vector
void LocalMolecule::updateAtoms(std::vector<Atom> &mainAtoms, int offset)
{
    throw(std::runtime_error("Can't updateAtoms() in a LocalMolecule.\n"));
}

// Return nth local atom
Atom &LocalMolecule::localAtom(int n) { return localAtoms_[n]; }
const Atom &LocalMolecule::localAtom(int n) const { return localAtoms_[n]; }

// Return local atoms
std::vector<Atom> &LocalMolecule::localAtoms() { return localAtoms_; }
const std::vector<Atom> &LocalMolecule::localAtoms() const { return localAtoms_; };
