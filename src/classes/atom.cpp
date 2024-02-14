// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atom.h"
#include "classes/atomVector.h"


/*
 * Atom Ref Implementation
 */

AtomRef::AtomRef() : index_(0), origin_(nullptr) {}
AtomRef::AtomRef(const AtomRef &ref) : index_(ref.index_), origin_(ref.origin_) {}
AtomRef::AtomRef(const std::size_t index, const AtomVector &source) : index_(index), origin_(const_cast<AtomVector *>(&source))
{
}

AtomRef AtomRef::operator[](std::size_t offset) { return AtomRef(index_ + offset, *origin_); }

bool AtomRef::operator==(const AtomRef &other) const { return index_ == other.index_; }
bool AtomRef::operator!=(const AtomRef &other) const { return index_ != other.index_ || origin_ != other.origin_; }

AtomRef &AtomRef::operator++()
{
    ++index_;
    return *this;
}

AtomRef AtomRef::operator++(int)
{
    auto original = index_++;
    return AtomRef(original, *origin_);
}

std::size_t AtomRef::operator-(const AtomRef &other) const { return index_ - other.index_; }

int AtomRef::globalAtomIndex() const { return index_; }

// Set SpeciesAtom that this Atom represents
void AtomRef::setSpeciesAtom(const SpeciesAtom *spAtom) { origin_->speciesAtom_[index_] = spAtom; }
// Set Molecule in which this Atom exists
void AtomRef::setMolecule(std::shared_ptr<Molecule> mol) { origin_->molecule_[index_] = mol; }
// Set local AtomType index
void AtomRef::setLocalTypeIndex(int id) { origin_->localTypeIndex_[index_] = id; }
// Return local AtomType index
int AtomRef::localTypeIndex() const { return origin_->localTypeIndex_[index_]; }
// Return master AtomType index
int AtomRef::masterTypeIndex() const { return origin_->masterTypeIndex_[index_]; }
// Set master AtomType index
void AtomRef::setMasterTypeIndex(int id) { origin_->masterTypeIndex_[index_] = id; }
// Add targeted potential to this atom
void AtomRef::addTargetedPotential(const ExternalPotential *potential)
{
    origin_->targetedPotentials_[index_].emplace_back(potential);
}
// Clear all targeted potentials from this Atom
void AtomRef::clearTargetedPotentials() { origin_->targetedPotentials_[index_].clear(); }
// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &AtomRef::targetedPotentials() const
{
  return origin_->targetedPotentials_[index_];
}
// Return coordinates
const Vec3<double> &AtomRef::r() const { return origin_->rs_[index_]; }

// Set coordinates
void AtomRef::setCoordinates(const Vec3<double> &newr) { origin_->rs_[index_] = newr; }

// Set cell in which the atom exists
void AtomRef::setCell(Cell *cell) { origin_->cell_[index_] = cell; }

// Return cell in which the atom exists
Cell *AtomRef::cell() const { return origin_->cell_[index_]; }

AtomRef &AtomRef::operator*() { return *this; }
const AtomRef &AtomRef::operator*() const { return *this; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *AtomRef::speciesAtom() const { return origin_->speciesAtom_[index_]; }

// Set coordinates
void AtomRef::setCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] = Vec3<double>(dx, dy, dz); }
// Translate coordinates
void AtomRef::translateCoordinates(const Vec3<double> &delta) { origin_->rs_[index_] += delta; }
// Translate coordinates
void AtomRef::translateCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] += Vec3<double>(dx, dy, dz); }
// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition AtomRef::scaling(const AtomRef j) const {
  assert(speciesAtom_[index_] != nullptr);
  assert(j.speciesAtom() != nullptr);

  return origin_->speciesAtom_[index_]->scaling(j.speciesAtom());
}
// Set coordinates
void AtomRef::set(const Vec3<double> r) { origin_->rs_[index_] = r; }
// Set coordinates
void AtomRef::set(double rx, double ry, double rz) { origin_->rs_[index_].set(rx, ry, rz); }
// Return x-coordinate
double AtomRef::x() const { return origin_->rs_[index_].x; }
// Return y-coordinate
double AtomRef::y() const { return origin_->rs_[index_].y; }
// Return z-coordinate
double AtomRef::z() const { return origin_->rs_[index_].z; }

// Return Molecule in which this Atom exists
const std::shared_ptr<Molecule> &AtomRef::molecule() const { return origin_->molecule_[index_]; }
