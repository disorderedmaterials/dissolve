// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/atom.h"
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
double Atom::scaling(std::shared_ptr<Atom> j) const
{
    assert(speciesAtom_ != nullptr);
    assert(j != nullptr);
    assert(j->speciesAtom() != nullptr);

    return speciesAtom_->scaling(j->speciesAtom());
}
