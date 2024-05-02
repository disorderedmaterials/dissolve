// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesRing.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"

SpeciesRing::SpeciesRing(const std::vector<const SpeciesAtom *> &atoms) : atoms_(atoms){};

bool SpeciesRing::operator==(const SpeciesRing &other) const
{
    // Check ring size first
    const auto nAtoms = atoms_.size();
    if (nAtoms != other.atoms_.size())
        return false;

    // Find equivalent atom in second ring to determine starting index
    auto indexA = 0;
    int indexB;
    for (indexB = 0; indexB < nAtoms; ++indexB)
        if (atoms_.at(indexA) == other.atoms_.at(indexB))
            break;

    // If we didn't find the atom, can return now
    if (indexB == nAtoms)
        return false;

    // Go over atoms and compare in both directions around the other ring
    for (auto n = 1; n < nAtoms; ++n)
    {
        if ((atoms_.at(indexA + n) != other.atoms_.at((indexB + n) % nAtoms)) &&
            (atoms_.at(indexA + n) != other.atoms_.at((nAtoms + indexB - n) % nAtoms)))
            return false;
    }

    return true;
}

/*
 * Atoms
 */

// Set atoms in ring
void SpeciesRing::setAtoms(const std::vector<const SpeciesAtom *> &atoms) { atoms_ = atoms; }

// Return nth atom in ring
const SpeciesAtom *SpeciesRing::atom(int n) const { return atoms_.at(n); }

// Return array of atoms in ring
const std::vector<const SpeciesAtom *> &SpeciesRing::atoms() const { return atoms_; }

// Return size of ring (number of atoms in array)
int SpeciesRing::size() const { return atoms_.size(); }

// Print ring information
void SpeciesRing::print() const
{
    std::string s = fmt::format("Ring({}) :", atoms_.size());
    for (const auto *atom : atoms_)
        s += fmt::format(" {}({})", atom->userIndex(), Elements::symbol(atom->Z()));
    Messenger::print(s);
}
