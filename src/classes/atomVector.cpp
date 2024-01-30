// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomVector.h"
#include "classes/atom.h"

AtomVector::AtomVector(int size) : atoms_(size), rs_(size) {}
std::size_t AtomVector::size() const { return atoms_.size(); }

AtomRef AtomVector::begin() { return AtomRef(0, *this); }
const AtomRef AtomVector::begin() const { return AtomRef(0, *this); }
AtomRef AtomVector::end() { return AtomRef(atoms_.size(), *this); }
const AtomRef AtomVector::end() const { return AtomRef(atoms_.size(), *this); }

AtomRef AtomVector::front() { return AtomRef(0, *this); }
AtomRef AtomVector::back() { return AtomRef(atoms_.size() - 1, *this); }
AtomRef AtomVector::operator[](std::size_t index) { return AtomRef(index, *this); }
const AtomRef AtomVector::operator[](std::size_t index) const { return AtomRef(index, *this); }

// Modifiers
void AtomVector::clear()
{
    atoms_.clear();
    rs_.clear();
}
void AtomVector::reserve(std::size_t newCapacity)
{
    atoms_.reserve(newCapacity);
    rs_.reserve(newCapacity);
}

AtomRef AtomVector::erase(const AtomRef pos)
{
    rs_.erase(rs_.begin() + pos.globalAtomIndex());
    auto result = atoms_.erase(atoms_.begin() + pos.globalAtomIndex());
    return AtomRef(result - atoms_.begin(), *this);
}
AtomRef AtomVector::erase(const AtomRef first, const AtomRef last)
{
    rs_.erase(rs_.begin() + first.globalAtomIndex(), rs_.begin() + last.globalAtomIndex());
    auto result = atoms_.erase(atoms_.begin() + first.globalAtomIndex(), atoms_.begin() + last.globalAtomIndex());
    return AtomRef(result - atoms_.begin(), *this);
}
AtomRef AtomVector::emplace_back()
{
    atoms_.emplace_back();
    rs_.emplace_back();
    return AtomRef(atoms_.size() - 1, *this);
}

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
void AtomRef::setSpeciesAtom(const SpeciesAtom *spAtom) { origin_->atoms_[index_].setSpeciesAtom(spAtom); }
// Set Molecule in which this Atom exists
void AtomRef::setMolecule(std::shared_ptr<Molecule> mol) { origin_->atoms_[index_].setMolecule(mol); }
// Set local AtomType index
void AtomRef::setLocalTypeIndex(int id) { origin_->atoms_[index_].setLocalTypeIndex(id); }
// Return local AtomType index
int AtomRef::localTypeIndex() const { return origin_->atoms_[index_].localTypeIndex(); }
// Return master AtomType index
int AtomRef::masterTypeIndex() const { return origin_->atoms_[index_].masterTypeIndex(); }
// Set master AtomType index
void AtomRef::setMasterTypeIndex(int id) { origin_->atoms_[index_].setMasterTypeIndex(id); }
// Add targeted potential to this atom
void AtomRef::addTargetedPotential(const ExternalPotential *potential)
{
    origin_->atoms_[index_].addTargetedPotential(potential);
}
// Clear all targeted potentials from this Atom
void AtomRef::clearTargetedPotentials() { origin_->atoms_[index_].clearTargetedPotentials(); }
// Return list of targeted potentials for this atom
const std::vector<const ExternalPotential *> &AtomRef::targetedPotentials() const
{
    return origin_->atoms_[index_].targetedPotentials();
}
// Return coordinates
const Vec3<double> &AtomRef::r() const { return origin_->rs_[index_]; }

// Set coordinates
void AtomRef::setCoordinates(const Vec3<double> &newr) { origin_->rs_[index_] = newr; }

// Set cell in which the atom exists
void AtomRef::setCell(Cell *cell) { origin_->atoms_[index_].cell_ = cell; }

// Return cell in which the atom exists
Cell *AtomRef::cell() const { return origin_->atoms_[index_].cell_; }

AtomRef &AtomRef::operator*() { return *this; }
const AtomRef &AtomRef::operator*() const { return *this; }

// Return SpeciesAtom that this Atom represents
const SpeciesAtom *AtomRef::speciesAtom() const { return origin_->atoms_[index_].speciesAtom(); }

// Set coordinates
void AtomRef::setCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] = Vec3<double>(dx, dy, dz); }
// Translate coordinates
void AtomRef::translateCoordinates(const Vec3<double> &delta) { origin_->rs_[index_] += delta; }
// Translate coordinates
void AtomRef::translateCoordinates(double dx, double dy, double dz) { origin_->rs_[index_] += Vec3<double>(dx, dy, dz); }
// Return scaling type and factors (electrostatic, van der Waals) to employ with specified Atom
SpeciesAtom::ScaledInteractionDefinition AtomRef::scaling(const AtomRef j) const { return origin_->atoms_[index_].scaling(j); }
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
const std::shared_ptr<Molecule> &AtomRef::molecule() const { return origin_->atoms_[index_].molecule(); }
