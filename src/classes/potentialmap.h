/*
    *** PotentialMap Definition
    *** src/classes/potentialmap.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "classes/atomtypelist.h"
#include "templates/array2d.h"

// Forward Declarations
class PairPotential;
class Atom;
class Molecule;
class SpeciesAtom;

// PotentialMap Definition
class PotentialMap
{
    public:
    PotentialMap();
    ~PotentialMap();
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
    bool initialise(const std::vector<std::shared_ptr<AtomType>> &masterAtomTypes, const List<PairPotential> &pairPotentials,
                    double pairPotentialRange);
    // Return PairPotential range
    double range() const;

    /*
     * Energy / Force
     */
    public:
    // Return energy between Atoms at distance specified
    double energy(const Atom *i, const Atom *j, double r) const;
    // Return energy between SpeciesAtoms at distance specified
    double energy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const;
    // Return analytic energy between Atom types at distance specified
    double analyticEnergy(const Atom *i, const Atom *j, double r) const;
    // Return force between Atoms at distance specified
    double force(const Atom *i, const Atom *j, double r) const;
    // Return force between SpeciesAtoms at distance specified
    double force(const SpeciesAtom *i, const SpeciesAtom *j, double r) const;
    // Return analytic force between Atom types at distance specified
    double analyticForce(const Atom *i, const Atom *j, double r) const;
};
