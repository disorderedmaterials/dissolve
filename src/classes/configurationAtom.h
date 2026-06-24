// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atom.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include <memory>
#include <vector>

// Forward Declarations
class Cell;
class Molecule;

// Configuration Atom
class ConfigurationAtom : public Atom<SpeciesBond>
{
    /*
     * Location
     */
    private:
    // SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom_{nullptr};
    // Molecule in which this Atom exists
    std::shared_ptr<Molecule> molecule_{nullptr};
    // Cell in which the atom exists
    Cell *cell_{nullptr};

    public:
    // Set SpeciesAtom that this Atom represents
    void setSpeciesAtom(const SpeciesAtom *spAtom);
    // Return SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom() const;
    // Set Molecule in which this Atom exists
    void setMolecule(std::shared_ptr<Molecule> mol);
    // Return Molecule in which this Atom exists
    const std::shared_ptr<Molecule> &molecule() const;
    // Set cell in which the atom exists
    void setCell(Cell *cell);
    // Return cell in which the atom exists
    Cell *cell() const;

    /*
     * Intramolecular Information
     */
    public:
    // Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
    SpeciesAtom::ScaledInteractionDefinition scaling(const ConfigurationAtom *j) const;
};
