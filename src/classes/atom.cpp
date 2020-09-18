/*
    *** Atom Definition
    *** src/classes/atom.cpp
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

#include "classes/atom.h"
#include "base/processpool.h"
#include "classes/atomtype.h"
#include "classes/speciesatom.h"

Atom::Atom() { clear(); }

Atom::~Atom() {}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void Atom::clear()
{
    molecule_ = nullptr;
    speciesAtom_ = nullptr;
    cell_ = nullptr;

    // Properties
    localTypeIndex_ = -1;
    masterTypeIndex_ = -1;
}

/*
 * Properties
 */

// Set coordinates
void Atom::set(const Vec3<double> r) { r_ = r; }

// Set coordinates
void Atom::set(double rx, double ry, double rz) { r_.set(rx, ry, rz); }

// Return coordinates
const Vec3<double> &Atom::r() const { return r_; }

// Return x-coordinate
double Atom::x() const { return r_.x; }

// Return y-coordinate
double Atom::y() const { return r_.y; }

// Return z-coordinate
double Atom::z() const { return r_.z; }

// Set local AtomType index
void Atom::setLocalTypeIndex(int id) { localTypeIndex_ = id; }

// Return local AtomType index
int Atom::localTypeIndex() const { return localTypeIndex_; }

// Set master AtomType index
void Atom::setMasterTypeIndex(int id)
{
    if (masterTypeIndex_ != -1)
        Messenger::warn("Warning: Overwriting master AtomType index for atom...\n");
    masterTypeIndex_ = id;
}

// Return master AtomType index
int Atom::masterTypeIndex() const
{
#ifdef CHECKS
    if (masterTypeIndex_ == -1)
        Messenger::warn("Global AtomType index has not yet been set for atom...\n");
#endif
    return masterTypeIndex_;
}

/*
 * Location
 */

// Set SpeciesAtom that this Atom represents
void Atom::setSpeciesAtom(const SpeciesAtom *spAtom) { speciesAtom_ = spAtom; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *Atom::speciesAtom() const { return speciesAtom_; }

// Set Molecule in which this Atom exists
void Atom::setMolecule(std::shared_ptr<Molecule> mol) { molecule_ = mol; }

// Return Molecule in which this Atom exists
std::shared_ptr<Molecule> Atom::molecule() const { return molecule_; }

// Set cell in which the atom exists
void Atom::setCell(Cell *cell) { cell_ = cell; }

// Return cell in which the atom exists
Cell *Atom::cell() const { return cell_; }

/*
 * Coordinate Manipulation
 */

// Set coordinates
void Atom::setCoordinates(const Vec3<double> &newr) { r_ = newr; }

// Set coordinates
void Atom::setCoordinates(double dx, double dy, double dz) { setCoordinates(Vec3<double>(dx, dy, dz)); }

// Translate coordinates
void Atom::translateCoordinates(const Vec3<double> &delta) { setCoordinates(r_ + delta); }

// Translate coordinates
void Atom::translateCoordinates(double dx, double dy, double dz) { setCoordinates(r_ + Vec3<double>(dx, dy, dz)); }

/*
 * Intramolecular Information
 */

// Return scaling factor to employ with specified Atom
double Atom::scaling(Atom *j) const
{
#ifdef CHECKS
    if (!speciesAtom_)
    {
        Messenger::error("Source SpeciesAtom pointer has not been set in Atom {}, so can't return scaling().\n", arrayIndex());
        return 0.0;
    }
    if (!j)
    {
        Messenger::error("Partner Atom 'j' not passed, so can't return scaling().\n");
        return 0.0;
    }
    if (!j->speciesAtom())
    {
        Messenger::error("SpeciesAtom pointer has not been set in partner Atom {}, so can't return scaling().\n",
                         j->arrayIndex());
        return 0.0;
    }
#endif
    return speciesAtom_->scaling(j->speciesAtom());
}
