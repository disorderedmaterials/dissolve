// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/pairPotential.h"
#include "templates/array2D.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesAtom;

// PotentialMap Definition
class PotentialMap
{
    public:
    PotentialMap() = default;
    ~PotentialMap() = default;
    // Clear all data
    void clear();

    /*
     * Source Parameters
     */
    private:
    // Number of unique types forming the matrix
    int nTypes_;
    // PairPotential matrix
    Array2D<PairPotential *> potentialMatrix_;
    // PairPotential range
    double range_;

    public:
    // Initialise map
    bool initialise(const std::vector<std::shared_ptr<AtomType>> &masterAtomTypes,
                    const std::vector<PairPotential::Definition> &pairPotentials, double pairPotentialRange);
    // Return PairPotential range
    double range() const;

    /*
     * Energy / Force
     */
    public:
    // Return energy between Atoms at distance specified
    double energy(const Atom &i, const Atom &j, double r) const;
    // Return energy between Atoms at distance specified, scaling electrostatic and short-range components
    double energy(const Atom &i, const Atom &j, double r, double elecScale, double srScale) const;
    // Return energy between SpeciesAtoms at distance specified
    double energy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const;
    // Return energy between SpeciesAtoms at distance specified, scaling electrostatic and short-range components
    double energy(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale, double srScale) const;
    // Return analytic energy between Atom types at distance specified
    double analyticEnergy(const Atom &i, const Atom &j, double r) const;
    // Return analytic energy between Atom types at distance specified, scaling electrostatic and short-range components
    double analyticEnergy(const Atom &i, const Atom &j, double r, double elecScale, double srScale) const;
    // Return force between Atoms at distance specified
    double force(const Atom &i, const Atom &j, double r) const;
    // Return force between Atoms at distance specified, scaling electrostatic and short-range components
    double force(const Atom &i, const Atom &j, double r, double elecScale, double srScale) const;
    // Return force between SpeciesAtoms at distance specified
    double force(const SpeciesAtom *i, const SpeciesAtom *j, double r) const;
    // Return force between SpeciesAtoms at distance specified, scaling electrostatic and short-range components
    double force(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale, double srScale) const;
    // Return analytic force between Atom types at distance specified
    double analyticForce(const Atom &i, const Atom &j, double r) const;
    // Return analytic force between Atom types at distance specified, scaling electrostatic and short-range components
    double analyticForce(const Atom &i, const Atom &j, double r, double elecScale, double srScale) const;
};
