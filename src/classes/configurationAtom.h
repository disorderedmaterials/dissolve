// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atom.h"
#include "classes/cell.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include <memory>

// Forward Declarations
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
    Cell<ConfigurationAtom> *cell_{nullptr};

    public:
    // Set SpeciesAtom that this Atom represents
    void setSpeciesAtom(const SpeciesAtom *spAtom);
    // Return SpeciesAtom that this Atom represents
    const SpeciesAtom *speciesAtom() const override;
    // Set Molecule in which this Atom exists
    void setMolecule(std::shared_ptr<Molecule> mol);
    // Return Molecule in which this Atom exists
    const std::shared_ptr<Molecule> &molecule() const override;

    /*
     * Intramolecular Information
     */
    public:
    // Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
    AtomBase::ScaledInteractionDefinition scaling(const AtomBase *j) const override;
};
