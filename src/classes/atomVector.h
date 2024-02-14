// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/speciesAtom.h"
#include "templates/vector3.h"
#include <iterator>
#include <vector>

class Atom;
class Atom;
class Cell;
class ExternalPotential;
class Molecule;
class SpeciesAtom;

class AtomVector
{
    private:
    std::vector<Vec3<double>> rs_;
    // Assigned AtomType index, local to Configuration (for partial indexing etc.)
    std::vector<int> localTypeIndex_;
    // Assigned master AtomType index (for pair potential indexing)
    std::vector<int> masterTypeIndex_;
    // SpeciesAtom that this Atom represents
    std::vector<const SpeciesAtom *>speciesAtom_{nullptr};
    // Molecule in which this Atom exists
    std::vector<std::shared_ptr<Molecule>> molecule_{nullptr};
    // Cell in which the atom exists
    std::vector<Cell *>cell_{nullptr};
    // Vector of targeted potentials affecting this atom
    std::vector<std::vector<const ExternalPotential *>> targetedPotentials_;

    public:
    AtomVector(int size = 0);
    std::size_t size() const;

    // Iterators
    Atom begin();
    Atom end();
    const Atom begin() const;
    const Atom end() const;

    // Accessors
    Atom front();
    Atom back();
    Atom operator[](std::size_t);
    const Atom operator[](std::size_t) const;

    // Modifiers
    void clear();
    void reserve(std::size_t newCapacity);
    Atom erase(const Atom pos);
    Atom erase(const Atom first, const Atom last);
    Atom emplace_back();
    friend class Atom;
};
