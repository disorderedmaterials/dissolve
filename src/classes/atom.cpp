// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/atomVector.h"


/*
 * Atom Ref Implementation
 */

Atom::Atom() : index_(0), origin_(nullptr) {}
Atom::Atom(const Atom &ref) : index_(ref.index_), origin_(ref.origin_) {}
Atom::Atom(const std::size_t index, const AtomVector &source) : index_(index), origin_(const_cast<AtomVector *>(&source))
{
}

Atom Atom::operator[](std::size_t offset) { return Atom(index_ + offset, *origin_); }

bool Atom::operator==(const Atom &other) const { return index_ == other.index_; }
bool Atom::operator!=(const Atom &other) const { return index_ != other.index_ || origin_ != other.origin_; }

Atom &Atom::operator++()
{
    ++index_;
    return *this;
}

Atom Atom::operator++(int)
{
    auto original = index_++;
    return Atom(original, *origin_);
}

std::size_t Atom::operator-(const Atom &other) const { return index_ - other.index_; }

int Atom::globalAtomIndex() const { return index_; }

// Set SpeciesAtom that this Atom represents
void Atom::setSpeciesAtom(const SpeciesAtom *spAtom) { origin_->speciesAtom_[index_] = spAtom; }
// Set Molecule in which this Atom exists
void Atom::setMolecule(std::shared_ptr<Molecule> mol) { origin_->molecule_[index_] = mol; }
// Set local AtomType index
void Atom::setLocalTypeIndex(int id) { origin_->localTypeIndex_[index_] = id; }
// Return local AtomType index
int Atom::localTypeIndex() const { return origin_->localTypeIndex_[index_]; }
// Return master AtomType index
int Atom::masterTypeIndex() const { return origin_->masterTypeIndex_[index_]; }
// Set master AtomType index
void Atom::setMasterTypeIndex(int id) { origin_->masterTypeIndex_[index_] = id; }
// Add targeted potential to this atom
void Atom::addTargetedPotential(const ExternalPotential *potential)
{
    origin_->targetedPotentials_[index_].emplace_back(potential);
}
// Clear all targeted potentials from this Atom
void Atom::clearTargetedPotentials() { origin_->targetedPotentials_[index_].clear(); }
// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &Atom::targetedPotentials() const
{
  return origin_->targetedPotentials_[index_];
}
// Return coordinates
const Vec3<double> &Atom::r() const { return origin_->rs_[index_]; }

// Set coordinates
void Atom::setCoordinates(const Vec3<double> &newr) { origin_->rs_[index_] = newr; }

// Set cell in which the atom exists
void Atom::setCell(Cell *cell) { origin_->cell_[index_] = cell; }

// Return cell in which the atom exists
Cell *Atom::cell() const { return origin_->cell_[index_]; }

Atom &Atom::operator*() { return *this; }
const Atom &Atom::operator*() const { return *this; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *Atom::speciesAtom() const { return origin_->speciesAtom_[index_]; }

// Set coordinates
void Atom::setCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] = Vec3<double>(dx, dy, dz); }
// Translate coordinates
void Atom::translateCoordinates(const Vec3<double> &delta) { origin_->rs_[index_] += delta; }
// Translate coordinates
void Atom::translateCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] += Vec3<double>(dx, dy, dz); }
// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition Atom::scaling(const Atom j) const {
  assert(speciesAtom_[index_] != nullptr);
  assert(j.speciesAtom() != nullptr);

  return origin_->speciesAtom_[index_]->scaling(j.speciesAtom());
}
// Set coordinates
void Atom::set(const Vec3<double> r) { origin_->rs_[index_] = r; }
// Set coordinates
void Atom::set(double rx, double ry, double rz) { origin_->rs_[index_].set(rx, ry, rz); }
// Return x-coordinate
double Atom::x() const { return origin_->rs_[index_].x; }
// Return y-coordinate
double Atom::y() const { return origin_->rs_[index_].y; }
// Return z-coordinate
double Atom::z() const { return origin_->rs_[index_].z; }

// Return Molecule in which this Atom exists
const std::shared_ptr<Molecule> &Atom::molecule() const { return origin_->molecule_[index_]; }
